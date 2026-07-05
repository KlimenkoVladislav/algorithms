#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <variant>
#include <optional>

struct Node{
    int axis;
    std::optional<int> bound;
    std::variant<std::monostate, Node*, Leaf*> left;
    std::variant<std::monostate, Node*, Leaf*> right;
};

struct Leaf{
    int first_leaf_index;
    int last_leaf_index;
    
    Leaf(int first, int last)
        : first_leaf_index(first), last_leaf_index(last) {}
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

    float distanse(const SparseVector &other) const {
        float distanse_value = 0.0;
        int this_nnz_coords = nnz_coords.size();
        int oth_nnz_coords = other.nnz_coords.size();
        int this_nnz_idx = 0;
        int oth_nnz_idx = 0;
        while ((this_nnz_idx < this_nnz_coords) 
           and (oth_nnz_idx < oth_nnz_coords)){
            if (nnz_coords_indices[this_nnz_idx] ==
                other.nnz_coords_indices[oth_nnz_idx]){
                    
                distanse_value += 
                (nnz_coords[nnz_coords_indices[this_nnz_idx]]
                - other.nnz_coords[other.nnz_coords_indices[oth_nnz_idx]]) 
                * (nnz_coords[nnz_coords_indices[this_nnz_idx]]
                - other.nnz_coords[other.nnz_coords_indices[oth_nnz_idx]]);
                
                this_nnz_idx++;
                oth_nnz_idx++;
            }
            else if (nnz_coords_indices[this_nnz_idx] <
                other.nnz_coords_indices[oth_nnz_idx]){
                
                distanse_value += 
                nnz_coords[nnz_coords_indices[this_nnz_idx]]
                * nnz_coords[nnz_coords_indices[this_nnz_idx]];
                this_nnz_idx++;
            }
            else {
                distanse_value +=
                other.nnz_coords[other.nnz_coords_indices[oth_nnz_idx]]
                * other.nnz_coords[other.nnz_coords_indices[oth_nnz_idx]];
                oth_nnz_idx++;
            }
        }

        while (this_nnz_idx < this_nnz_coords){
            distanse_value += 
            nnz_coords[nnz_coords_indices[this_nnz_idx]]
            * nnz_coords[nnz_coords_indices[this_nnz_idx]];

            this_nnz_idx++;
        }
        while (oth_nnz_idx < oth_nnz_coords){
            distanse_value +=
            other.nnz_coords[other.nnz_coords_indices[oth_nnz_idx]]
            * other.nnz_coords[other.nnz_coords_indices[oth_nnz_idx]];

            oth_nnz_idx++;
        }

        return distanse_value;
    }
};

class KDTree{
private:
    std::variant<std::monostate, Node*, Leaf*> _root;
    std::vector<SparseVector*> _db;
    std::vector<int> _data_indices;
    int _leaf_max_size;

    std::optional<int> finding_median(int left_bound_indices,
                                      int right_bound_indices,
                                      int &axis){
        
        if (left_bound_indices == right_bound_indices){
            return std::nullopt;
        }
        
        // сделано для оптимизации: подсчитывает, у скольких векторов есть
        // значение по этой оси
        int bound_nnz_num = (right_bound_indices - 
                                        left_bound_indices) / 2 + 1;
        while (true){
            int non_zero_count = 0;
            for (int i = left_bound_indices; i <= right_bound_indices; i++){
                int idx = _data_indices[i];
                if (_db[idx]->checkValue(axis)){
                    non_zero_count++;
                    if (non_zero_count > bound_nnz_num){
                        bound_nnz_num = -1;
                        break;
                    }
                }
            }
            if (bound_nnz_num != -1){ break; }
            else { axis = (axis + 1) % _db[0]->dimension; }
        }

        int median = (left_bound_indices + right_bound_indices) / 2;
        auto nth_index = _data_indices.begin() + median;
        
        std::nth_element(_data_indices.begin() + left_bound_indices,
                         nth_index,
                         _data_indices.begin() + right_bound_indices+1,
                         [&](int I, int J){
                             return _db[I]->getAxisValue(axis) 
                                  < _db[J]->getAxisValue(axis);
                         });

        return _db[_data_indices[median]]->getAxisValue(axis);
    }

    std::variant<std::monostate, Node*, Leaf*> KDTreeCreate(
                                    int left_bound_indices,
                                    int right_bound_indices, 
                                    int axis){
        
        if (left_bound_indices >= right_bound_indices){
            return std::monostate{};
        }
        if (right_bound_indices - left_bound_indices + 1 
                                                    > _leaf_max_size){
            Node *new_node = new Node();
            new_node->bound = finding_median(left_bound_indices, 
                                             right_bound_indices, axis);
            if (!new_node->bound.has_value()){ return std::monostate{}; }
            new_node->axis = axis;

            int median = (left_bound_indices + 
                          right_bound_indices) / 2;
            int next_axis = (axis + 1) % _db[0]->dimension;

            new_node->left = KDTreeCreate(left_bound_indices, median,
                                          next_axis);
            new_node->right = KDTreeCreate(median + 1, right_bound_indices,
                                           next_axis);
            
            return new_node;
        }
        else {
            return new Leaf(left_bound_indices, right_bound_indices);
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
                _root = KDTreeCreate(0, _db.size()-1, 0);
           }

    static KDTree createTree(int leaf_max_size, 
                             std::vector<SparseVector*> &db){
        std::vector<int> data_indices(db.size());
        int db_size = db.size();
        for (int i = 0; i < db_size; i++){
            data_indices[i] = i;
        }

        return KDTree(std::move(db), leaf_max_size, std::move(data_indices));
    }

    std::vector<int> topKRetrieval(std::vector<int> &query, int k){

    }

    ~KDTree(){
        clearTree(_root);
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
        sparse_vector->dimension = index + 1;
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

    KDTree tree = KDTree::createTree(leaf_max_size, db);

    std::vector<int> query_vector;
    std::string query_string;
    std::cout << "Введите query вектор для поиска похожих" 
              << " (координаты должны быть введены через пробел):\n";
    std::getline(std::cin, query_string);
    std::string num;
    for (int i = 0; i < query_string.size(); i++){
        if ((query_string[i] == ' ') and (!num.empty())){
            query_vector.push_back(std::stoi(num));
            num.clear();
        }
        else if ((query_string[i] == '0') and (num.empty())){
            query_vector.push_back(0);
        }
        else { num.push_back(query_string[i]); }
    }
    query_string.clear();

    int k;
    std::cout << "Введите k для top-k vector retrieval: ";
    std::cin >> k;

    tree.topKRetrieval(query_vector, k);
}