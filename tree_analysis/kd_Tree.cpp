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
    int first_index_in_leaf;
    int last_index_in_leaf;
    
    Leaf(int first, int last)
        : first_index_in_leaf(first), last_index_in_leaf(last) {}
};

struct SparseVector{
    std::vector<int> indices_non_zero_koordinats;
    std::vector<int> non_zero_koordinats;
    int dimension;

    int getAxisValue(int axis) const {
        auto it = std::lower_bound(indices_non_zero_koordinats.begin(),
                                   indices_non_zero_koordinats.end(),
                                   axis);
        if ((it != indices_non_zero_koordinats.end()) and (*it == axis)){
            int position = it - indices_non_zero_koordinats.begin();
            return non_zero_koordinats[position];
        }
        return 0;
    }

    bool checkValue(int axis) const {
        auto it = std::lower_bound(indices_non_zero_koordinats.begin(),
                                   indices_non_zero_koordinats.end(),
                                   axis);
        return (it != indices_non_zero_koordinats.end()) and (*it == axis);
    }

    float distanse(const SparseVector &other_vector) const {
        int distanse_value = 0;
        for (int axis = 0; axis < dimension; axis++){
            int this_vector_value = getAxisValue(axis);
            if (this_vector_value){
                int other_vector_value = other_vector.getAxisValue(axis);
                if (other_vector_value){
                    distanse_value += (this_vector_value - other_vector_value) 
                                    * (this_vector_value - other_vector_value);
                }
            }
        }

        return distanse_value;
    }
};

class kdTree{
private:
    std::variant<std::monostate, Node*, Leaf*> _root;
    std::vector<SparseVector*> _db;
    std::vector<int> _data_indices;
    int _leaf_max_size;

    std::optional<int> finding_median(int left_bound_for_indexes,
                                      int right_bound_for_indexes,
                                      int axis){
        
        if (left_bound_for_indexes == right_bound_for_indexes){
            return std::nullopt;
        }
        
        int median = (left_bound_for_indexes + right_bound_for_indexes) / 2;
        auto nth_index = _data_indices.begin() + median;
        
    }

    std::variant<std::monostate, Node*, Leaf*> kdTreeCreate(
                                    int left_bound_for_indexes,
                                    int right_bound_for_indexes, 
                                    int axis){
        
        if (left_bound_for_indexes >= right_bound_for_indexes){
            return std::monostate{};
        }
        if (right_bound_for_indexes - left_bound_for_indexes + 1 
                                                    > _leaf_max_size){
            Node *new_node = new Node();
            new_node->axis = axis;
            new_node->bound = finding_median(left_bound_for_indexes, 
                                             right_bound_for_indexes, axis);
            
            if (!new_node->bound.has_value()){ return std::monostate{}; }

            int median = (left_bound_for_indexes + 
                          right_bound_for_indexes) / 2;
            int next_axis = (axis + 1) % _db[0]->dimension;

            new_node->left = kdTreeCreate(left_bound_for_indexes, median,
                                          next_axis);
            new_node->right = kdTreeCreate(median + 1, right_bound_for_indexes,
                                           next_axis);
            
            return new_node;
        }
        else {
            return new Leaf(left_bound_for_indexes, right_bound_for_indexes);
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
    kdTree(std::vector<SparseVector*> db, 
           int leaf_max_size,
           std::vector<int> data_indices) 
         : _db(std::move(db)), 
           _data_indices(std::move(data_indices)), 
           _leaf_max_size(leaf_max_size){
                _root = kdTreeCreate(0, _db.size()-1, 0);
           }

    static kdTree createTree(int leaf_max_size, 
                             std::vector<SparseVector*> &db){
        std::vector<int> data_indices(db.size());
        int db_size = db.size();
        for (int i = 0; i < db_size; i++){
            data_indices[i] = i;
        }

        return kdTree(std::move(db), leaf_max_size, std::move(data_indices));
    }

    std::vector<int> topKRetrieval(std::vector<int> &query, int k){

    }

    ~kdTree(){
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
                sparse_vector->non_zero_koordinats.push_back(std::stoi(num));
                sparse_vector->indices_non_zero_koordinats.push_back(index);
                num.clear();
                index++;
            }
            else if ((line[i] == '0') and (num.empty())){
                index++;
            }
            else { num.push_back(line[i]); }
        }
        if (!num.empty()){
            sparse_vector->non_zero_koordinats.push_back(std::stoi(num));
            sparse_vector->indices_non_zero_koordinats.push_back(index);
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

    kdTree tree = kdTree::createTree(leaf_max_size, db);

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