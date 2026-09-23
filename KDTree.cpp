#include "utilities.cpp"

using namespace std;
ofstream fout1("results\\tree_nodes.csv");
ofstream fout2("results\\query_result.csv");

class Node{
    public:
        int depth;
        Node *parent_node;
        Node *left;
        Node *right;
        Movie movie;

    Node(){
        this->depth = 0;
        this->parent_node = nullptr;
        this->left = nullptr;
        this->right = nullptr;
        this->movie = Movie();
    }
};


class KDTree{
    public:
        Node *root;

    KDTree(){
        root = nullptr;
    }

    KDTree(Node *root){
        this->root = root;
    }

    KDTree(ifstream &fin){
        // Reading the movies from the CSV file and building the KDTree
        this->root = nullptr;
        int length = getCsvLength(fin);
        vector<Movie> movies;
        readCsvMovies(fin, movies);
        Node **nodes = new Node*[length];
        for(int i=0; i<length; i++){
            nodes[i] = new Node();
            nodes[i]->movie = Movie(movies[i]);
        }
        Build(this->root, nullptr, nodes, 0, length-1, 0, "");
    }

    void Build(Node *local_root_node, Node *parent_node, Node **nodes, int left, int right, int dim, string direction);

    void Insert(Movie new_movie);

    void Delete(Movie movie, Node *subtree_root = nullptr);

    void Update(Movie old_movie, Movie new_movie);

    Node* Search(Movie movie, Node *subtree_root = nullptr);

    void RangeSearch(Node *v, vector<Node> &result, int min_runtime, int max_runtime, double min_vote_average, double max_vote_average, double min_popularity, double max_popularity, vector<string> original_languages, Date min_release_date, Date max_release_date);
    
    void findSubtreeMin(Node *v, Node *min, int dim);

    void findSubtreeMax(Node *v, Node *max, int dim);

    void printElements(Node *v);
};


void KDTree::Build(Node *local_root_node, Node *parent_node, Node **nodes, int left, int right, int dim, string direction){
    // Sorting the nodes based on the current dimension to find the median node
    sort(nodes + left, nodes + right + 1, [dim](Node *a, Node *b){
        if(dim == 0) return a->movie.runtime < b->movie.runtime;
        else if(dim == 1) return a->movie.vote_average < b->movie.vote_average;
        else if(dim == 2) return a->movie.popularity < b->movie.popularity;
        else if(dim == 3) return a->movie.original_language < b->movie.original_language;
        else if(dim == 4) return a->movie.release_date < b->movie.release_date;
    });
    
    // Finding the median node to be the root of the current subtree
    int i = (left+right)/2;
    while(i < right){
        if(dim == 0){
            if(nodes[i]->movie.runtime == nodes[i+1]->movie.runtime)
                i++;
            else
                break;
        }else if(dim == 1){
            if(nodes[i]->movie.vote_average == nodes[i+1]->movie.vote_average)
                i++;
            else
                break;
        }else if(dim == 2){
            if(nodes[i]->movie.popularity == nodes[i+1]->movie.popularity)
                i++;
            else
                break;
        }else if(dim == 3){
            if(nodes[i]->movie.original_language == nodes[i+1]->movie.original_language)
                i++;
            else
                break;
        }else if(dim == 4){
            if(nodes[i]->movie.release_date == nodes[i+1]->movie.release_date)
                i++;
            else
                break;
        }
    }
    local_root_node = nodes[i];

    // Setting the parent node and depth of the current subtree root node
    if(parent_node != nullptr){
        if(direction == "left"){
            parent_node->left = local_root_node;
        }else if(direction == "right"){
            parent_node->right = local_root_node;
        }
    }else{
        this->root = local_root_node;
    }

    local_root_node->parent_node = parent_node;
    if(parent_node == nullptr){
        local_root_node->depth = 0;
    }else{
        local_root_node->depth = parent_node->depth + 1;
    }
    
    // Recursively building the left and right subtrees of the current subtree root node if there are nodes remaining in the left and right halves of the current subtree
    if(left < i)
        Build(local_root_node->left, local_root_node, nodes, left, i-1, (dim+1)%5, "left");
    if(right > i)
        Build(local_root_node->right, local_root_node, nodes, i+1, right, (dim+1)%5, "right");
}


void KDTree::Insert(Movie new_movie){
    // Creating the new node with the given movie
    Node *new_element = new Node();
    new_element->movie = Movie(new_movie);

    Node *v = this->root;
    Node *temp;
    int dim;

    // Traversing the tree to find the appropriate position for the new movie based on its indexing attributes
    while(v != nullptr){
        dim = v->depth % 5;
        temp = v;

        if(dim == 0){
            if(new_movie.runtime <= v->movie.runtime){
                v = v->left; 
            }else{
                v = v->right;
            }
        }else if(dim == 1){
            if(new_movie.vote_average <= v->movie.vote_average){
                v = v->left;
            }else{
                v = v->right;
            }
        }else if(dim == 2){
            if(new_movie.popularity <= v->movie.popularity){
                v = v->left;
            }else{
                v = v->right;
            }
        }else if(dim == 3){
            if(new_movie.original_language <= v->movie.original_language){
                v = v->left;
            }else{
                v = v->right;
            }
        }else if(dim == 4){
            if(new_movie.release_date <= v->movie.release_date){
                v = v->left;
            }else{
                v = v->right;
            }
        }
    }

    if(v == this->root){
        this->root = new_element;
        return;
    }

    // Inserting the new node as a child of the appropriate parent node that found before
    if(dim == 0){
        if(new_movie.runtime <= temp->movie.runtime){
            temp->left = new_element; 
        }else{
            temp->right = new_element;
        }
    }else if(dim == 1){
        if(new_movie.vote_average <= temp->movie.vote_average){
            temp->left = new_element;
        }else{  
            temp->right = new_element;
        }
    }else if(dim == 2){
        if(new_movie.popularity <= temp->movie.popularity){
            temp->left = new_element;
        }else{
            temp->right = new_element;
        }

    }else if(dim == 3){
        if(new_movie.original_language <= temp->movie.original_language){
            temp->left = new_element;
        }else{
            temp->right = new_element;
        }
    }else if(dim == 4){
        if(new_movie.release_date <= temp->movie.release_date){
            temp->left = new_element;
        }else{
            temp->right = new_element;
        }
    }

    // Setting the parent node and depth of the new node
    new_element->parent_node = temp;
    new_element->depth = temp->depth + 1;
}


void KDTree::Delete(Movie movie, Node *subtree_root){
    // Searching the movie in the tree
    Node *v = this->Search(movie, subtree_root);

    if(v != nullptr){
        if(v->left == nullptr && v->right == nullptr){
            // If the movie is in a leaf node, the node is deleted
            if(v->parent_node->left == v){
                v->parent_node->left = nullptr;
            }else{
                v->parent_node->right = nullptr;
            }
            (*v).~Node();
        }else if(v->right != nullptr){
            // If the movie is in a node with a right subtree, the minimum node of the right subtree is found and its movie is copied to the current node, then the minimum node is deleted
            Node *min = new Node();
            findSubtreeMin(v->right, min, v->depth % 5);
            v->movie.id = min->movie.id;
            v->movie.title = min->movie.title;
            v->movie.runtime = min->movie.runtime;
            v->movie.vote_average = min->movie.vote_average;
            v->movie.popularity = min->movie.popularity;
            v->movie.original_language = min->movie.original_language;
            v->movie.release_date = min->movie.release_date;
            v->movie.production_companies = vector<string>(min->movie.production_companies);
            Delete(movie, v->right);
        }else{
            // If the movie is in a node with a left subtree, the maximum node of the left subtree is found and its movie is copied to the current node, then the maximum node is deleted
            Node *max = new Node();
            findSubtreeMax(v->left, max, v->depth % 5);
            v->movie.id = max->movie.id;
            v->movie.title = max->movie.title;
            v->movie.runtime = max->movie.runtime;
            v->movie.vote_average = max->movie.vote_average;
            v->movie.popularity = max->movie.popularity;
            v->movie.original_language = max->movie.original_language;
            v->movie.release_date = max->movie.release_date;
            v->movie.production_companies = vector<string>(max->movie.production_companies);
            Delete(movie, v->left);
        }
    }
}


void KDTree::Update(Movie old_movie, Movie new_movie){
    // The movie is deleted from the tree and the updated movie is re-inserted into the tree
    Delete(old_movie);
    Insert(new_movie);
}


Node* KDTree::Search(Movie movie, Node *subtree_root){
    Node *v;
    if(subtree_root == nullptr){
        v = this->root;
    }else{
        v = subtree_root;
    }

    // Traversing the tree to find the node containing the movie based on its indexing attributes
    while(v != nullptr){
        if(v->movie == movie){
            return v;
        }

        int dim = v->depth % 5;
        if(dim == 0){
            if(movie.runtime <= v->movie.runtime){
                v = v->left;
            }else{
                v = v->right;
            }
        }else if(dim == 1){
            if(movie.vote_average <= v->movie.vote_average){
                v = v->left;
            }else{
                v = v->right;
            }
        }else if(dim == 2){
            if(movie.popularity <= v->movie.popularity){
                v = v->left;
            }else{
                v = v->right;
            }
        }else if(dim == 3){
            if(movie.original_language <= v->movie.original_language){
                v = v->left;
            }else{
                v = v->right;
            }
        }else if(dim == 4){
            if(movie.release_date <= v->movie.release_date){
                v = v->left;
            }else{
                v = v->right;
            }
        }
    }

    return nullptr;
}


void KDTree::RangeSearch(Node *v, vector<Node> &result, int min_runtime, int max_runtime, double min_vote_average, double max_vote_average, double min_popularity, double max_popularity, vector<string> original_languages, Date min_release_date, Date max_release_date){
    // Checking if the current node's movie is within the specified range of indexing attributes and adding it to the result vector if it is
    if(v->movie.inRange(min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date)){
        result.push_back(*v);
    }

    // Traversing the left and right subtrees of the current node based on the current dimension
    // If the left or right subtree may contain movies within the specified range, the function is called recursively on that subtree
    int dim = v->depth % 5;
    if(dim == 0){
        if(min_runtime <= v->movie.runtime && v->left != nullptr){
            RangeSearch(v->left, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
        }
        if(max_runtime > v->movie.runtime && v->right != nullptr){
            RangeSearch(v->right, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);        }
    }else if(dim == 1){
        if(min_vote_average <= v->movie.vote_average && v->left != nullptr){
            RangeSearch(v->left, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
        }
        if(max_vote_average > v->movie.vote_average && v->right != nullptr){
            RangeSearch(v->right, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
        }
    }else if(dim == 2){
        if(min_popularity <= v->movie.popularity && v->left != nullptr){
            RangeSearch(v->left, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
        }
        if(max_popularity > v->movie.popularity && v->right != nullptr){
            RangeSearch(v->right, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
        }
    }else if(dim == 3){
        if(find(original_languages.begin(), original_languages.end(), "*") != original_languages.end()){
            if(v->left != nullptr){
                RangeSearch(v->left, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
            }
            if(v->right != nullptr){
                RangeSearch(v->right, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
            }
        }else{
            for(int i=0; i<original_languages.size(); i++){
                if(original_languages[i] <= v->movie.original_language && v->left != nullptr){
                    RangeSearch(v->left, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
                    break;
                }
            }

            for(int i=0; i<original_languages.size(); i++){
                if(original_languages[i] > v->movie.original_language && v->right != nullptr){
                    RangeSearch(v->right, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
                    break;
                }
            }
        }
    }else if(dim == 4){
        if(min_release_date <= v->movie.release_date && v->left != nullptr){
            RangeSearch(v->left, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
        }
        if(max_release_date > v->movie.release_date && v->right != nullptr){
            RangeSearch(v->right, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
        }
    }
}


void KDTree::findSubtreeMin(Node *v, Node *min, int dim){
    // In-order traversal of a subtree with root node v, in order to find the node with the minimum value of the selected dimension
    if(v == nullptr){
        return;
    }

    findSubtreeMin(v->left, min, dim);

    if(dim == 0){
        if(v->movie.runtime < min->movie.runtime){
            min = v;
        }
    }else if(dim == 1){
        if(v->movie.vote_average < min->movie.vote_average){
            min = v;
        }
    }else if(dim == 2){
        if(v->movie.popularity < min->movie.popularity){
            min = v;
        }
    }else if(dim == 3){
        if(v->movie.original_language < min->movie.original_language){
            min = v;
        }
    }else if(dim == 4){
        if(v->movie.release_date < min->movie.release_date){
            min = v;
        }
    }

    findSubtreeMin(v->right, min, dim);
}


void KDTree::findSubtreeMax(Node *v, Node *max, int dim){
    // In-order traversal of a subtree with root node v, in order to find the node with the maximum value of the selected dimension
    if(v == nullptr){
        return;
    }

    findSubtreeMax(v->left, max, dim);

    if(dim == 0){
        if(v->movie.runtime > max->movie.runtime){
            max = v;
        }
    }else if(dim == 1){
        if(v->movie.vote_average > max->movie.vote_average){
            max = v;
        }
    }else if(dim == 2){
        if(v->movie.popularity > max->movie.popularity){
            max = v;
        }
    }else if(dim == 3){
        if(v->movie.original_language > max->movie.original_language){
            max = v;
        }
    }else if(dim == 4){
        if(v->movie.release_date > max->movie.release_date){
            max = v;
        }
    }

    findSubtreeMax(v->right, max, dim);
}


void KDTree::printElements(Node *v){
    // Printing the elements of the tree in an in-order traversal manner to a CSV file
    if(v == this->root){
        fout1 << "id;title;runtime;vote_average;popularity;original_language;release_date;production_companies" << endl;
    }

    if(v == nullptr){
        return;
    }
    
    printElements(v->left);
    fout1 << v->movie.id << ";" << v->movie.title << ";" << v->movie.runtime << ";" << v->movie.vote_average << ";"<< v->movie.popularity << ";" << v->movie.original_language << ";" << v->movie.release_date.toString() << ";";
    fout1 << "[";
    for(int i=0; i<v->movie.production_companies.size(); i++){
        fout1 << "\'" << v->movie.production_companies[i] << "\'";
        if(i != v->movie.production_companies.size()-1){
            fout1 << ",";
        }
    }
    fout1 << "]" << endl;
    printElements(v->right);
}


void queryResultToCsv(vector<Node> result){
    // Writing a query result to a CSV file
    fout2 << "id;title;runtime;vote_average;popularity;original_language;release_date;production_companies" << endl;
    for(int i=0; i<result.size(); i++){
        fout2 << result[i].movie.id << ";" << result[i].movie.title << ";" << result[i].movie.runtime << ";" << result[i].movie.vote_average << ";" << result[i].movie.popularity << ";" << result[i].movie.original_language << ";" << result[i].movie.release_date.toString() << ";";
        fout2 << "[";
        for(int j=0; j<result[i].movie.production_companies.size(); j++){
            fout2 << "\'" << result[i].movie.production_companies[j] << "\'";
            if(j != result[i].movie.production_companies.size()-1){
                fout2 << ",";
            }
        }
        fout2 << "]" << endl;
    }
}


int main(){
    ifstream fin("movies csv data\\data_movies_clean.csv");

    auto start = std::chrono::high_resolution_clock::now();
    KDTree tree = KDTree(fin);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    cout << "KDTree Construction Time: " << duration.count() << " seconds" << std::endl;

    tree.printElements(tree.root);

    int min_runtime = 0;
    int max_runtime = 100;
    double min_vote_average = 6.0;
    double max_vote_average = 10.0;
    double min_popularity = 2.0;
    double max_popularity = 8000.0;
    vector<string> original_languages = {"en"};
    Date min_release_date = Date("2019-01-01");
    Date max_release_date = Date("2020-12-31");

    vector<Node> result;
    tree.RangeSearch(tree.root, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);

    cout << "Query Result Size: " << result.size() << endl;
    queryResultToCsv(result);
}
