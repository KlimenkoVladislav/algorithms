#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <variant>
#include <optional>
#include <queue>
#include <cmath>
#include <limits>

struct Leaf{
    int first_leaf_index;
    int last_leaf_index;
    
    Leaf(int first, int last)
        : first_leaf_index(first), last_leaf_index(last) {}
};

struct Node{
    int axis;
    std::optional<int> bound;
    std::variant<std::monostate, Node*, Leaf*> left;
    std::variant<std::monostate, Node*, Leaf*> right;
};

struct SparseVector{
    std::vector<int> nnz_coords_indices;
    std::vector<int> nnz_coords;
    int dimension;

    int getAxisValue(int axis) const {
        auto it = std::lower_bound(nnz_coords_indices.begin(),
                                   nnz_coords_indices.end(),
                                   axis);
        if ((it != nnz_coords_indices.end()) and (*it == axis)){
            int position = it - nnz_coords_indices.begin();
            return nnz_coords[position];
        }
        return 0;
    }

    bool checkValue(int axis) const {
        auto it = std::lower_bound(nnz_coords_indices.begin(),
                                   nnz_coords_indices.end(),
                                   axis);
        return (it != nnz_coords_indices.end()) and (*it == axis);
    }

    int distance(const SparseVector &other) const {
        int distance_value = 0;
        int this_nnz_coords = nnz_coords.size();
        int oth_nnz_coords = other.nnz_coords.size();
        int i = 0;
        int j = 0;
        while ((i < this_nnz_coords) 
           and (j < oth_nnz_coords)){
            if (nnz_coords_indices[i] == other.nnz_coords_indices[j]){
                int diff = nnz_coords[i] - other.nnz_coords[j];
                distance_value += diff * diff;
                i++; j++;
            }
            else if (nnz_coords_indices[i] < other.nnz_coords_indices[j]){
                distance_value += nnz_coords[i] * nnz_coords[i];
                i++;
            }
            else {
                distance_value += other.nnz_coords[j] * other.nnz_coords[j];
                j++;
            }
        }

        while (i < this_nnz_coords){
            distance_value += nnz_coords[i] * nnz_coords[i];
            i++;
        }
        while (j < oth_nnz_coords){
            distance_value += other.nnz_coords[j] * other.nnz_coords[j];
            j++;
        }

        return distance_value;
    }
};

class KDTree{
private:
    std::variant<std::monostate, Node*, Leaf*> _root;
    std::vector<SparseVector*> _db;
    std::vector<int> _data_indices;
    int _leaf_max_size;

    struct Candidate{
        float distance;
        int index;

        bool operator<(const Candidate &other) const {
            return distance < other.distance;
        }
    };

    std::optional<int> findMedian(const int left_bound_indices,
                                  const int right_bound_indices,
                                  int &axis){
        
        if (left_bound_indices == right_bound_indices){
            return std::nullopt;
        }
        
        // сделано для оптимизации: подсчитывает, у скольких векторов есть
        // значение по этой оси
        int bound_zero_val = (right_bound_indices - 
                                        left_bound_indices) / 2 + 1;
        while (true){
            int non_zero_count = 0;
            for (int i = left_bound_indices; i <= right_bound_indices; i++){
                int idx = _data_indices[i];
                if (_db[idx]->checkValue(axis)){
                    non_zero_count++;
                    if (non_zero_count > bound_zero_val){
                        bound_zero_val = -1;
                        break;
                    }
                }
            }
            if (bound_zero_val != -1){ break; }
            else { axis = (axis + 1) % _db[0]->dimension; }
        }

        int median = (left_bound_indices + right_bound_indices) / 2;
        auto nth_index = _data_indices.begin() + median;
        
        std::nth_element(_data_indices.begin() + left_bound_indices,
                         nth_index,
                         _data_indices.begin() + right_bound_indices+1,
                         [&](int I, int J){
                            int valI = _db[I]->getAxisValue(axis);
                            int valJ = _db[J]->getAxisValue(axis);
                            if (valI != valJ) {
                                return valI < valJ;
                            }
                            return I < J;
                         });

        return _db[_data_indices[median]]->getAxisValue(axis);
    }

    std::variant<std::monostate, Node*, Leaf*> buildTree(
                                    const int left_bound_indices,
                                    const int right_bound_indices, 
                                    int axis){
        
        if (left_bound_indices >= right_bound_indices){
            return std::monostate{};
        }
        if (right_bound_indices - left_bound_indices + 1 > _leaf_max_size){
            Node *new_node = new Node();
            new_node->bound = findMedian(left_bound_indices, 
                                             right_bound_indices, axis);
            if (!new_node->bound.has_value()){ return std::monostate{}; }
            new_node->axis = axis;

            int median = (left_bound_indices + right_bound_indices) / 2;
            int next_axis = (axis + 1) % _db[0]->dimension;

            new_node->left = buildTree(left_bound_indices, median,
                                          next_axis);
            new_node->right = buildTree(median + 1, right_bound_indices,
                                           next_axis);
            
            return new_node;
        }
        else {
            return new Leaf(left_bound_indices, right_bound_indices);
        }
    }

    void searchRecursive(
        const std::variant<std::monostate, Node*, Leaf*> &node,
        const SparseVector &query,
        int k,
        std::priority_queue<Candidate> &heap,
        float &best_dist){

        if (std::holds_alternative<std::monostate>(node)){ return; }
        if (std::holds_alternative<Leaf*>(node)){
            Leaf *leaf = std::get<Leaf*>(node);
            for (int i = leaf->first_leaf_index; i <= leaf->last_leaf_index; i++){
                int idx = _data_indices[i];
                float dist = query.distance(*_db[idx]);

                if (heap.size() < k){
                    heap.push({dist, idx});
                    best_dist = heap.top().distance;
                }
                else if (dist < heap.top().distance){
                    heap.pop();
                    heap.push({dist, idx});
                    best_dist = heap.top().distance;
                }
            }
            return;
        }

        Node *n = std::get<Node*>(node);
        int query_val = query.getAxisValue(n->axis);
        int bound = n->bound.value();

        bool go_left = (query_val <= bound);
        if (go_left){
            searchRecursive(n->left, query, k, heap, best_dist);
        }
        else {
            searchRecursive(n->right, query, k, heap, best_dist);
        }

        // backtracking
        float dist_to_plane = std::abs(query_val - bound);
        if ((dist_to_plane < best_dist) or (heap.size() < k)){
            if (go_left){
                searchRecursive(n->right, query, k, heap, best_dist);
            }
            else {
                searchRecursive(n->left, query, k, heap, best_dist);
            }
        }
    }

    void clearTree(std::variant<std::monostate, Node*, Leaf*> &node){
        if (std::holds_alternative<Node*>(node)) {
            Node* n = std::get<Node*>(node);
            clearTree(n->left);
            clearTree(n->right);
            delete n;
        } else if (std::holds_alternative<Leaf*>(node)) {
            delete std::get<Leaf*>(node);
        }
        node = std::monostate{};
    }

public:
    KDTree(std::vector<SparseVector*> db, 
           int leaf_max_size,
           std::vector<int> data_indices) 
         : _db(std::move(db)), 
           _data_indices(std::move(data_indices)), 
           _leaf_max_size(leaf_max_size){
                _root = buildTree(0, _db.size()-1, 0);
           }

    static KDTree create(int leaf_max_size, 
                             std::vector<SparseVector*> &db){
        std::vector<int> data_indices(db.size());
        int db_size = db.size();
        for (int i = 0; i < db_size; i++){
            data_indices[i] = i;
        }

        return KDTree(std::move(db), leaf_max_size, std::move(data_indices));
    }

    void topKRetrieval(SparseVector &query, int k){
        // по хорошему, надо, чтобы функция что-то возвращала, но так как это
        // не боевой проект, а чисто для анализа, то и так сойдёт)
        if ((k <= 0) or _db.empty()){ return; }

        std::priority_queue<Candidate> heap;
        float best_dist = std::numeric_limits<float>::infinity();

        searchRecursive(_root, query, k, heap, best_dist);

        std::vector<Candidate> results;
        while (!heap.empty()){
            results.push_back(heap.top());
            heap.pop();
        }
        std::reverse(results.begin(), results.end());

        for (const auto &c : results) {
            std::cout << "Вектор " << c.index << ": ";
            
            const auto* vec = _db[c.index];
            int current_index = 0;
            
            for (size_t i = 0; i < vec->nnz_coords_indices.size(); ++i) {
                int coord_index = vec->nnz_coords_indices[i];
                int value = vec->nnz_coords[i];
                
                while (current_index < coord_index) {
                    std::cout << "0 ";
                    ++current_index;
                }

                std::cout << value << " ";
                ++current_index;
            }
            
            while (current_index < vec->dimension) {
                std::cout << "0 ";
                ++current_index;
            }

            std::cout << "\n";
        }
    }

    ~KDTree(){
        clearTree(_root);

        for (auto vec : _db){
            delete vec;
        }
        _db.clear();
    }
};

void readDatabase(std::vector<SparseVector*> &db){
    std::ifstream file("database.db");
    if (!file.is_open()){
        std::cout << "Не удалось открыть файл\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)){
        int line_length = line.length();
        std::string num;
        int index = 0;
        SparseVector *sparse_vector = new SparseVector();
        for (int i = 0; i < line_length; i++){
            if ((line[i] == ' ') and (!num.empty())){
                sparse_vector->nnz_coords.push_back(std::stoi(num));
                sparse_vector->nnz_coords_indices.push_back(index);
                num.clear();
                index++;
            }
            else if (line[i] == ' '){ continue; }
            else if ((line[i] == '0') and (num.empty())){
                index++;
            }
            else { num.push_back(line[i]); }
        }
        if (!num.empty()){
            sparse_vector->nnz_coords.push_back(std::stoi(num));
            sparse_vector->nnz_coords_indices.push_back(index);
            num.clear();
            index++;
        }
        sparse_vector->dimension = index;
        db.push_back(sparse_vector);
        index = 0;
    }
    
    file.close();
}

int main(){
    std::vector<SparseVector*> db;
    readDatabase(db);
    if (db.empty()){ return -1; }

    int leaf_max_size;
    std::cout << "Введите максимальный размер листа k-d tree: ";
    std::cin >> leaf_max_size;
    std::cin.ignore();

    KDTree tree = KDTree::create(leaf_max_size, db);

    SparseVector *query = new SparseVector();
    std::string query_string;
    std::cout << "Введите query вектор для поиска похожих" 
              << " (координаты должны быть введены через пробел):\n";
    std::getline(std::cin, query_string);
    std::string num;
    int index = 0;
    for (int i = 0; i < query_string.size(); i++){
        if ((query_string[i] == ' ') and (!num.empty())){
            query->nnz_coords.push_back(std::stoi(num));
            query->nnz_coords_indices.push_back(index);
            num.clear();
            index++;
        }
        else if (query_string[i] == ' '){ continue; }
        else if ((query_string[i] == '0') and (num.empty())){
            index++;
        }
        else { num.push_back(query_string[i]); }
    }
    if (!num.empty()){
        query->nnz_coords.push_back(std::stoi(num));
        query->nnz_coords_indices.push_back(index);
        num.clear();
        index++;
    }
    query_string.clear();
    query->dimension = index;

    int k;
    std::cout << "Введите k для top-k vector retrieval: ";
    std::cin >> k;

    tree.topKRetrieval(*query, k);

    delete query;
}