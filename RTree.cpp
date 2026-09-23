#include "utilities.cpp"

#define MAX_NODE_CAPACITY 8
#define MAX_LEAF_CAPACITY 16
#define MIN_LEAF_CAPACITY (MAX_LEAF_CAPACITY/2)
#define MIN_NODE_CAPACITY (MAX_NODE_CAPACITY/2)

using namespace std;
ofstream fout1("results\\tree_nodes.csv");
ofstream fout2("results\\query_result.csv");

class MBR{
    public:
        // An MBR, defined by the minimum and maximum values of the indexing attributes
        int min_runtime, max_runtime;
        double min_vote_average, max_vote_average;
        double min_popularity, max_popularity;
        unsigned long min_original_language, max_original_language;
        unsigned long min_release_date, max_release_date;

    MBR(){
        this->min_runtime = MAX_INT; 
        this->max_runtime = 0;
        this->min_vote_average = MAX_DOUBLE;
        this->max_vote_average = 0;
        this->min_popularity = MAX_DOUBLE;
        this->max_popularity = 0;
        this->min_original_language = MAX_ULONG;
        this->max_original_language = 0;
        this->min_release_date = MAX_ULONG;
        this->max_release_date = 0;
    }

    MBR(int min_runtime, int max_runtime, double min_vote_average, double max_vote_average, double min_popularity, double max_popularity, vector<string> original_languages, Date min_release_date, Date max_release_date){
        this->min_runtime = min_runtime;
        this->max_runtime = max_runtime;
        this->min_vote_average = min_vote_average;
        this->max_vote_average = max_vote_average;
        this->min_popularity = min_popularity;
        this->max_popularity = max_popularity;

        if(find(original_languages.begin(), original_languages.end(), "*") != original_languages.end()){
            this->min_original_language = 0;
            this->max_original_language = MAX_ULONG;
        }else if(original_languages.size() == 1){
            this->min_original_language = stringToInt(original_languages[0]);
            this->max_original_language = stringToInt(original_languages[0]);
        }else{
            sort(original_languages.begin(), original_languages.end());
            this->min_original_language = stringToInt(original_languages[0]);
            this->max_original_language = stringToInt(original_languages[original_languages.size()-1]);
        }
        
        this->min_release_date = min_release_date.toNumber();
        this->max_release_date = max_release_date.toNumber();
    }

    double getArea(){
        // Calculating the area of the MBR by multiplying the lengths of each dimension, while handling cases where dimensions are equal to avoid zero area
        double area = 1;
        bool allDimensionsEqual = true;

        (this->max_runtime - this->min_runtime) == 0 ? area *= 1 : (area *= (this->max_runtime - this->min_runtime), allDimensionsEqual = false);
        (this->max_vote_average - this->min_vote_average) == 0 ? area *= 1 : (area *= (this->max_vote_average - this->min_vote_average), allDimensionsEqual = false);
        (this->max_popularity - this->min_popularity) == 0 ? area *= 1 : (area *= (this->max_popularity - this->min_popularity), allDimensionsEqual = false);
        (this->max_original_language - this->min_original_language) == 0 ? area *= 1 : (area *= (this->max_original_language - this->min_original_language), allDimensionsEqual = false);
        (this->max_release_date - this->min_release_date) == 0 ? area *= 1 : (area *= (this->max_release_date - this->min_release_date), allDimensionsEqual = false);

        if(allDimensionsEqual){
            area = 0;
        }

        return area;
    }

    void updateMBR(Movie movie){
        // Updating the MBR to include a new Movie object by adjusting the minimum and maximum values
        this->min_runtime = min(this->min_runtime, movie.runtime);
        this->max_runtime = max(this->max_runtime, movie.runtime);
        this->min_vote_average = min(this->min_vote_average, movie.vote_average);
        this->max_vote_average = max(this->max_vote_average, movie.vote_average);
        this->min_popularity = min(this->min_popularity, movie.popularity);
        this->max_popularity = max(this->max_popularity, movie.popularity);
        this->min_original_language = min(this->min_original_language, stringToInt(movie.original_language));
        this->max_original_language = max(this->max_original_language, stringToInt(movie.original_language));
        this->min_release_date = min(this->min_release_date, movie.release_date.toNumber());
        this->max_release_date = max(this->max_release_date, movie.release_date.toNumber());
    }

    void updateMBR(MBR mbr){
        // Updating the MBR to include another MBR by adjusting the minimum and maximum values
        this->min_runtime = min(this->min_runtime, mbr.min_runtime);
        this->max_runtime = max(this->max_runtime, mbr.max_runtime);
        this->min_vote_average = min(this->min_vote_average, mbr.min_vote_average);
        this->max_vote_average = max(this->max_vote_average, mbr.max_vote_average);
        this->min_popularity = min(this->min_popularity, mbr.min_popularity);
        this->max_popularity = max(this->max_popularity, mbr.max_popularity);
        this->min_original_language = min(this->min_original_language, mbr.min_original_language);
        this->max_original_language = max(this->max_original_language, mbr.max_original_language);
        this->min_release_date = min(this->min_release_date, mbr.min_release_date);
        this->max_release_date = max(this->max_release_date, mbr.max_release_date);
    }

    bool contains(Movie movie){
        // Checking if the MBR contains the given movie by comparing the Movie's attributes with the MBR's minimum and maximum values
        if(movie.runtime >= this->min_runtime && movie.runtime <= this->max_runtime &&
           movie.vote_average >= this->min_vote_average && movie.vote_average <= this->max_vote_average &&
           movie.popularity >= this->min_popularity && movie.popularity <= this->max_popularity &&
           stringToInt(movie.original_language) >= this->min_original_language && stringToInt(movie.original_language) <= this->max_original_language &&
           movie.release_date.toNumber() >= this->min_release_date && movie.release_date.toNumber() <= this->max_release_date)
        {
            return true;
        }

        return false;
    }

    bool covers(MBR mbr){
        // Checking if the MBR covers another MBR, making all the appropriate comparisons
        if(mbr.min_runtime <= this->max_runtime && mbr.max_runtime >= this->min_runtime &&
           mbr.min_vote_average <= this->max_vote_average && mbr.max_vote_average >= this->min_vote_average &&
           mbr.min_popularity <= this->max_popularity && mbr.max_popularity >= this->min_popularity &&
           mbr.min_original_language <= this->max_original_language && mbr.max_original_language >= this->min_original_language &&
           mbr.min_release_date <= this->max_release_date && mbr.max_release_date >= this->min_release_date)
        {
            return true;
        }

        return false;
    }
};


class Node{
    public:
        bool is_leaf;
        Node *parent_node;
        vector<MBR> mbrs;
        vector<Node*> children;
        vector<Movie> movies;

    Node(){
        this->is_leaf = false;
        this->parent_node = nullptr;
        this->mbrs = vector<MBR>();
        this->children = vector<Node*>();
        this->movies = vector<Movie>();
    }

    Node(bool is_leaf) : Node(){
        this->is_leaf = is_leaf;
    }

    double getMaxMBRsDistance(string dim){
        // Calculating the maximum normalized distance between the MBRs of the node based on the specified dimension, by finding the highest minimum and lowest maximum values of the MBRs in that dimension
        double normalized_distance = 0;

        if(dim == "runtime"){
            int max = this->mbrs[0].max_runtime;
            int min = this->mbrs[0].min_runtime;
            int highest_min = this->mbrs[0].min_runtime;
            int lowest_max = this->mbrs[0].max_runtime;

            for(int i=1; i<this->mbrs.size(); i++){
                if(this->mbrs[i].max_runtime > max){
                    max = this->mbrs[i].max_runtime;
                }
                if(this->mbrs[i].min_runtime < min){
                    min = this->mbrs[i].min_runtime;
                }
                if(this->mbrs[i].min_runtime > highest_min){
                    highest_min = this->mbrs[i].min_runtime;
                }
                if(this->mbrs[i].max_runtime < lowest_max){
                    lowest_max = this->mbrs[i].max_runtime;
                }
            }

            int distance = lowest_max - highest_min;
            if(max - min != 0){
                normalized_distance = (double)distance / (max - min);
            }else{
                normalized_distance = MIN_DOUBLE;
            }

        }else if(dim == "vote_average"){
            double max = this->mbrs[0].max_vote_average;
            double min = this->mbrs[0].min_vote_average;
            double highest_min = this->mbrs[0].min_vote_average;
            double lowest_max = this->mbrs[0].max_vote_average;

            for(int i=1; i<this->mbrs.size(); i++){
                if(this->mbrs[i].max_vote_average > max){
                    max = this->mbrs[i].max_vote_average;
                }
                if(this->mbrs[i].min_vote_average < min){
                    min = this->mbrs[i].min_vote_average;
                }
                if(this->mbrs[i].min_vote_average > highest_min){
                    highest_min = this->mbrs[i].min_vote_average;
                }
                if(this->mbrs[i].max_vote_average < lowest_max){
                    lowest_max = this->mbrs[i].max_vote_average;
                }
            }

            double distance = lowest_max - highest_min;
            if(max - min != 0){
                normalized_distance = (double)distance / (max - min);
            }else{
                normalized_distance = MIN_DOUBLE;
            }

        }else if(dim == "popularity"){
            double max = this->mbrs[0].max_popularity;
            double min = this->mbrs[0].min_popularity;
            double highest_min = this->mbrs[0].min_popularity;
            double lowest_max = this->mbrs[0].max_popularity;

            for(int i=1; i<this->mbrs.size(); i++){
                if(this->mbrs[i].max_popularity > max){
                    max = this->mbrs[i].max_popularity;
                }
                if(this->mbrs[i].min_popularity < min){
                    min = this->mbrs[i].min_popularity;
                }
                if(this->mbrs[i].min_popularity > highest_min){
                    highest_min = this->mbrs[i].min_popularity;
                }
                if(this->mbrs[i].max_popularity < lowest_max){
                    lowest_max = this->mbrs[i].max_popularity;
                }
            }

            double distance = lowest_max - highest_min;
            if(max - min != 0){
                normalized_distance = (double)distance / (max - min);
            }else{
                normalized_distance = MIN_DOUBLE;
            }
            
        }else if(dim == "original_language"){
            unsigned long max = this->mbrs[0].max_original_language;
            unsigned long min = this->mbrs[0].min_original_language;
            unsigned long highest_min = this->mbrs[0].min_original_language;
            unsigned long lowest_max = this->mbrs[0].max_original_language;

            for(int i=1; i<this->mbrs.size(); i++){
                if(this->mbrs[i].max_original_language > max){
                    max = this->mbrs[i].max_original_language;
                }
                if(this->mbrs[i].min_original_language < min){
                    min = this->mbrs[i].min_original_language;
                }
                if(this->mbrs[i].min_original_language > highest_min){
                    highest_min = this->mbrs[i].min_original_language;
                }
                if(this->mbrs[i].max_original_language < lowest_max){
                    lowest_max = this->mbrs[i].max_original_language;
                }
            }

            unsigned long distance = lowest_max - highest_min;
            if(max - min != 0){
                normalized_distance = (double)distance / (max - min);
            }else{
                normalized_distance = MIN_DOUBLE;
            }

        }else if(dim == "release_date"){
            unsigned long max = this->mbrs[0].max_release_date;
            unsigned long min = this->mbrs[0].min_release_date;
            unsigned long highest_min = this->mbrs[0].min_release_date;
            unsigned long lowest_max = this->mbrs[0].max_release_date;

            for(int i=1; i<this->mbrs.size(); i++){
                if(this->mbrs[i].max_release_date > max){
                    max = this->mbrs[i].max_release_date;
                }
                if(this->mbrs[i].min_release_date < min){
                    min = this->mbrs[i].min_release_date;
                }
                if(this->mbrs[i].min_release_date > highest_min){
                    highest_min = this->mbrs[i].min_release_date;
                }
                if(this->mbrs[i].max_release_date < lowest_max){
                    lowest_max = this->mbrs[i].max_release_date;
                }
            }

            unsigned long distance = lowest_max - highest_min;
            if(max - min != 0){
                normalized_distance = (double)distance / (max - min);
            }else{
                normalized_distance = MIN_DOUBLE;
            }
        }

        return normalized_distance;  
    }

    vector<int> chooseMovieSeeds(){
        // Choosing two movies that are the farthest apart based on their distance, to serve as seeds for splitting the node
        vector<int> seeds;
        double max_distance = -1;

        // Iterating through all pairs of movies in the node to find the pair with the maximum distance
        for(int i=0; i<this->movies.size()-1; i++){
            for(int j=i+1; j<this->movies.size(); j++){
                double distance = this->movies[i].getDistance(this->movies[j]);
                if(distance > max_distance){
                    max_distance = distance;
                    seeds.clear();
                    seeds.push_back(i);
                    seeds.push_back(j);
                }
            }
        }
        return seeds;
    }

    vector<int> chooseMBRSeeds(){
        // Choosing two MBRs that are the farthest apart based on their distance, to serve as seeds for splitting the node
        vector<int> seeds;

        // Finding the maximum distance for each dimension among the MBRs in the node
        double runtime_distance = this->getMaxMBRsDistance("runtime");
        double vote_average_distance = this->getMaxMBRsDistance("vote_average");
        double popularity_distance = this->getMaxMBRsDistance("popularity");
        double original_language_distance = this->getMaxMBRsDistance("original_language");
        double release_date_distance = this->getMaxMBRsDistance("release_date");

        // Determining the maximum distance among all dimensions to identify which dimension to use for selecting seeds
        double max_distance = max({runtime_distance, vote_average_distance, popularity_distance, original_language_distance, release_date_distance});
        seeds.push_back(-1);
        seeds.push_back(-1);

        // Selecting seeds based on the dimension with the maximum distance, by finding the MBRs with the highest minimum and lowest maximum values in that dimension
        if(max_distance == runtime_distance){
            int highest_min = 0;
            int lowest_max = MAX_INT;

            for(int i=0; i<this->mbrs.size(); i++){
                if(this->mbrs[i].min_runtime >= highest_min){
                    highest_min = this->mbrs[i].min_runtime;
                    seeds[0] = i;
                }
            }

            for(int i=0; i<this->mbrs.size(); i++){
                if(i == seeds[0]){
                    continue;
                }

                if(this->mbrs[i].max_runtime <= lowest_max){
                    lowest_max = this->mbrs[i].max_runtime;
                    seeds[1] = i;
                }
            }

        }else if(max_distance == vote_average_distance){
            double highest_min = 0;
            double lowest_max = MAX_DOUBLE;
            
            for(int i=0; i<this->mbrs.size(); i++){
                if(this->mbrs[i].min_vote_average >= highest_min){
                    highest_min = this->mbrs[i].min_vote_average;
                    seeds[0] = i;
                }
            }

            for(int i=0; i<this->mbrs.size(); i++){
                if(i == seeds[0]){
                    continue;
                }

                if(this->mbrs[i].max_vote_average <= lowest_max){
                    lowest_max = this->mbrs[i].max_vote_average;
                    seeds[1] = i;
                }
            }
            
        }else if(max_distance == popularity_distance){
            double highest_min = 0;
            double lowest_max = MAX_DOUBLE;

            for(int i=0; i<this->mbrs.size(); i++){
                if(this->mbrs[i].min_popularity >= highest_min){
                    highest_min = this->mbrs[i].min_popularity;
                    seeds[0] = i;
                }
            }

            for(int i=0; i<this->mbrs.size(); i++){
                if(i == seeds[0]){
                    continue;
                }

                if(this->mbrs[i].max_popularity <= lowest_max){
                    lowest_max = this->mbrs[i].max_popularity;
                    seeds[1] = i;
                }
            }
            
        }else if(max_distance == original_language_distance){
            unsigned long highest_min = 0;
            unsigned long lowest_max = MAX_ULONG;
            
            for(int i=0; i<this->mbrs.size(); i++){
                if(this->mbrs[i].min_original_language >= highest_min){
                    highest_min = this->mbrs[i].min_original_language;
                    seeds[0] = i;
                }
            }

            for(int i=0; i<this->mbrs.size(); i++){
                if(i == seeds[0]){
                    continue;
                }

                if(this->mbrs[i].max_original_language <= lowest_max){
                    lowest_max = this->mbrs[i].max_original_language;
                    seeds[1] = i;
                }
            }
        }else if(max_distance == release_date_distance){
            unsigned long highest_min = 0;
            unsigned long lowest_max = MAX_ULONG;

            for(int i=0; i<this->mbrs.size(); i++){
                if(this->mbrs[i].min_release_date >= highest_min){
                    highest_min = this->mbrs[i].min_release_date;
                    seeds[0] = i;
                }
            }

            for(int i=0; i<this->mbrs.size(); i++){
                if(i == seeds[0]){
                    continue;
                }

                if(this->mbrs[i].max_release_date <= lowest_max){
                    lowest_max = this->mbrs[i].max_release_date;
                    seeds[1] = i;
                }
            }
        }

        return seeds;
    }
};


class RTree{
    public:
        Node *root;

    RTree(){
        this->root = nullptr;
    }

    RTree(ifstream &fin){
        // Initializing the RTree by reading movies from a CSV file and inserting them into the tree
        this->root = nullptr;
        vector<Movie> movies;
        readCsvMovies(fin, movies);
        for(int i=0; i<movies.size(); i++){
            Insert(movies[i]);
        }
    }

    void Insert(Movie movie);

    void Delete(Movie movie);

    void Update(Movie old_movie, Movie new_movie);

    void Search(Movie movie, Movie &result, Node *subtree_root = nullptr);

    void Search(Movie movie, Node *&result, Node *subtree_root = nullptr);

    void RangeSearch(Node *v, vector<Movie> &result, MBR search_mbr, vector<string> original_languages);

    void Split(Node *v);

    void Share(Node *v1, Node *v2);

    void Merge(Node *v);

    Node* chooseSubtree(Node *v, Movie movie);

    void printElements(Node *v);
};


void RTree::Insert(Movie movie){
    // If the root is null, creating a new root node and inserting the movie
    if(this->root == nullptr){
        this->root = new Node(true);
        this->root->movies.push_back(movie);
        return;
    }

    // In any other case, traversing the tree to find the appropriate leaf node for insertion
    Node *v = this->root;
    while(!v->is_leaf){
        v = chooseSubtree(v, movie);
    }
    
    // Inserting the movie into the leaf node and updating the MBRs of the parent nodes until reaching the root
    v->movies.push_back(movie);
    if(v != this->root){
        Node *u = v;
        while(u != this->root){
            for(int i=0; i<u->parent_node->children.size(); i++){
                if(u == u->parent_node->children[i]){
                    u->parent_node->mbrs[i].updateMBR(movie);
                    break;
                }
            }
            u = u->parent_node;
        }
    }
    
    // If the leaf node exceeds the maximum capacity, splitting the node to maintain the R-tree structure properties
    if(v->movies.size() > MAX_LEAF_CAPACITY){
        Split(v);
    }
} 


void RTree::Delete(Movie movie){
    // Searching for the node containing the movie to be deleted
    Node *v;
    Search(movie, v);

    // If the movie is found in a leaf node, removing it from the node's movie list
    for(int i=0; i<v->movies.size(); i++){
        if(v->movies[i] == movie){
            v->movies.erase(v->movies.begin() + i);
        }
    }

    // Updating the MBRs of the parent nodes until the root to reflect the deletion of the movie, by recalculating the MBRs based on the remaining movies in the node
    for(int i=0; i<v->parent_node->children.size(); i++){
        if(v->parent_node->children[i] == v){
            v->parent_node->mbrs[i] = MBR();
            for(int j=0; j<v->movies.size(); j++){
                v->parent_node->mbrs[i].updateMBR(v->movies[j]);
            }
            
            Node *u = v->parent_node;
            while(u != this->root){
                for(int i=0; i<u->parent_node->children.size(); i++){
                    if(u->parent_node->children[i] == u){
                        u->parent_node->mbrs[i] = MBR();
                        for(int j=0; j<u->mbrs.size(); j++){
                            u->parent_node->mbrs[i].updateMBR(u->mbrs[j]);
                        }
                    }
                }
                u = u->parent_node;
            }
            break;
        }
    }

    // If the leaf node falls below the minimum capacity, attempting to share movies with sibling nodes or merging nodes to maintain the R-tree structure properties
    if(v->movies.size() < MIN_LEAF_CAPACITY){
        bool shared = false;
        for(int i=0; i<v->parent_node->children.size(); i++){
            if(v->parent_node->children[i] == v){
                continue;
            }

            // If a sibling node has more than the minimum leaf capacity, sharing movies between the nodes to balance their sizes
            if(v->parent_node->children[i]->movies.size() > MIN_LEAF_CAPACITY){
                Share(v, v->parent_node->children[i]);
                shared = true;
                break;
            }
        }

        // If sharing is not possible, merging the node with a sibling node and checking the parent node for underflow,
        // repeating the same process up the tree until the structure properties are restored or the root is reached
        if(!shared){
            Merge(v);
            v = v->parent_node;
            while(v != this->root && v->children.size() < MIN_NODE_CAPACITY){
                shared = false;
                for(int i=0; i<v->parent_node->children.size(); i++){
                    if(v->parent_node->children[i] == v){
                        continue;
                    }
        
                    if(v->parent_node->children[i]->movies.size() > MIN_NODE_CAPACITY){
                        Share(v, v->parent_node->children[i]);
                        shared = true;
                        break;
                    }
                }

                if(!shared){
                    Merge(v);
                }
                v = v->parent_node;
            }
        }
    }
}


void RTree::Update(Movie old_movie, Movie new_movie){
    // The movie is deleted from the tree and the updated movie is re-inserted into the tree
    Delete(old_movie);
    Insert(new_movie);
}


void RTree::Search(Movie movie, Movie &result, Node *subtree_root){
    Node *v;
    if(subtree_root == nullptr){
        v = this->root;
    }else{
        v = subtree_root;
    }

    if(v->is_leaf){
        // If the current node is a leaf, iterating through its movies to find a match with the given movie and storing it in the argument result
        for(int i=0; i<v->movies.size(); i++){
            if(v->movies[i] == movie){
                result = v->movies[i];
            }
        }
    }else{
        // If the current node is not a leaf, iterating through its children and checking if their MBRs may contain the given movie, recursively searching in those children
        for(int i=0; i<v->children.size(); i++){
            if(v->mbrs[i].contains(movie)){
                Search(movie, result, v->children[i]);
            }
        }
    }
}


void RTree::Search(Movie movie, Node *&result, Node *subtree_root){
    // Same with the previous Search function, but instead of returning the movie, it returns the node containing the movie
    Node *v;
    if(subtree_root == nullptr){
        v = this->root;
    }else{
        v = subtree_root;
    }

    if(v->is_leaf){
        for(int i=0; i<v->movies.size(); i++){
            if(v->movies[i] == movie){
                result = v;
            }
        }
    }else{
        for(int i=0; i<v->children.size(); i++){
            if(v->mbrs[i].contains(movie)){
                Search(movie, result, v->children[i]);
            }
        }
    }
}


void RTree::RangeSearch(Node *v, vector<Movie> &result, MBR search_mbr, vector<string> original_languages){
    if(v->is_leaf){
        // If the current node is a leaf, adding the movies that are contained in the search MBR and have an original language that matches the specified languages to the result vector
        for(int i=0; i<v->movies.size(); i++){
            Movie movie(v->movies[i]);
            if(search_mbr.contains(movie) && (find(original_languages.begin(), original_languages.end(), "*") != original_languages.end() || find(original_languages.begin(), original_languages.end(), movie.original_language) != original_languages.end())){
                result.push_back(v->movies[i]);
            }
        }
    }else{
        // If the current node is not a leaf, recursively searching its children whose MBRs cover the search MBR
        for(int i=0; i<v->mbrs.size(); i++){
            if(v->mbrs[i].covers(search_mbr)){
                RangeSearch(v->children[i], result, search_mbr, original_languages);
            }
        }
    }
}


void RTree::Split(Node *v){
    // Splitting the node v into two new nodes v1 and v2
    Node *v1 = new Node();
    Node *v2 = new Node();
    
    if(v->is_leaf){
        // Using the chooseMovieSeeds() function to select two movies as seeds for the split
        vector<int> seeds = v->chooseMovieSeeds();
        v1->is_leaf = true;
        v2->is_leaf = true;

        // Adding the selected seed movies to the new nodes and updating their MBRs accordingly
        v1->movies.push_back(v->movies[seeds[0]]);
        v2->movies.push_back(v->movies[seeds[1]]);
        MBR mbr1, mbr2;
        mbr1.updateMBR(v->movies[seeds[0]]);
        mbr2.updateMBR(v->movies[seeds[1]]);
        
        // Distributing the remaining movies between the two new nodes based on which node would require less enlargement of its MBR to accommodate the movie
        for(int i=0; i<v->movies.size(); i++){
            if(i == seeds[0]  || i == seeds[1]){
                continue;
            }

            // Calculating the enlargement required for each node's MBR to include the current movie
            MBR temp_mbr1(mbr1);
            temp_mbr1.updateMBR(v->movies[i]);
            double enlargement1 = temp_mbr1.getArea() - mbr1.getArea();

            MBR temp_mbr2(mbr2);
            temp_mbr2.updateMBR(v->movies[i]);
            double enlargement2 = temp_mbr2.getArea() - mbr2.getArea();

            // Assigning the movie to the node that requires less enlargement of its MBR, and updating the MBR accordingly
            if(enlargement1 <= enlargement2){
                v1->movies.push_back(v->movies[i]);
                mbr1 = temp_mbr1;
            }else{
                v2->movies.push_back(v->movies[i]);
                mbr2 = temp_mbr2;
            }
        }
        
        if(v != this->root){
            // If the node being split is not the root, updating the parent nodes of v1 and v2
            v1->parent_node = v->parent_node;
            v2->parent_node = v->parent_node;

            // Finding the index of the original node v in its parent's children vector, removing it, and inserting v1 and v2 in its place, along with their corresponding MBRs
            for(int i=0; i<v->parent_node->children.size(); i++){
                if(v == v->parent_node->children[i]){
                    v->parent_node->children.erase(v->parent_node->children.begin() + i);
                    v->parent_node->children.insert(v->parent_node->children.begin() + i, {v1, v2});

                    v->parent_node->mbrs.erase(v->parent_node->mbrs.begin() + i);
                    v->parent_node->mbrs.insert(v->parent_node->mbrs.begin() + i, {mbr1, mbr2});
                    break;
                }
            }

            // If either of the new nodes is below the minimum leaf capacity, sharing movies with the sibling node to balance their sizes
            if(v1->movies.size() < MIN_LEAF_CAPACITY){
                Share(v1, v2);
            }else if(v2->movies.size() < MIN_LEAF_CAPACITY){
                Share(v2, v1);
            }
        }else{
            // If the node being split is the root, creating a new root node and assigning v1 and v2 as its children, updating their parent_node pointers accordingly
            Node *new_root = new Node(false);
            new_root->children = {v1, v2};
            new_root->mbrs = {mbr1, mbr2};
            this->root = new_root;
            v1->parent_node = this->root;
            v2->parent_node = this->root;
        }

    }else{
        // If the node being split is not a leaf, using the chooseMBRSeeds() function to select two MBRs as seeds for the split
        vector<int> seeds = v->chooseMBRSeeds();  
        v1->is_leaf = false;
        v2->is_leaf = false;

        // Adding the selected seed MBRs and their corresponding child nodes to the new nodes and updating their MBRs accordingly
        v1->mbrs.push_back(v->mbrs[seeds[0]]);
        v1->children.push_back(v->children[seeds[0]]);
        v2->mbrs.push_back(v->mbrs[seeds[1]]);
        v2->children.push_back(v->children[seeds[1]]);
        MBR mbr1, mbr2;
        mbr1.updateMBR(v->mbrs[seeds[0]]);
        mbr2.updateMBR(v->mbrs[seeds[1]]);
        
        // Distributing the remaining MBRs and their corresponding child nodes between the two new nodes based on which node would require less enlargement of its MBR to accommodate the MBR
        for(int i=0; i<v->mbrs.size(); i++){
            if(i == seeds[0] || i == seeds[1]){
                continue;
            }

            // Calculating the enlargement required for each node's MBR to include the current MBR
            MBR temp_mbr1(mbr1);
            temp_mbr1.updateMBR(v->mbrs[i]);
            double enlargement1 = temp_mbr1.getArea() - mbr1.getArea();

            MBR temp_mbr2(mbr2);
            temp_mbr2.updateMBR(v->mbrs[i]);
            double enlargement2 = temp_mbr2.getArea() - mbr2.getArea();

            // Assigning the MBR and its corresponding child node to the node that requires less enlargement of its MBR
            if(enlargement1 <= enlargement2){
                v1->mbrs.push_back(v->mbrs[i]);
                v1->children.push_back(v->children[i]);
                mbr1 = temp_mbr1;
            }else{
                v2->mbrs.push_back(v->mbrs[i]);
                v2->children.push_back(v->children[i]);
                mbr2 = temp_mbr2;
            }
        }
        
        // Updating the parent_node pointers of the child nodes in v1 and v2 to point to their new parent nodes
        for(int i=0; i<v1->children.size(); i++){
            v1->children[i]->parent_node = v1;
        }
        for(int i=0; i<v2->children.size(); i++){
            v2->children[i]->parent_node = v2;
        }
       
        if(v != this->root){
            // If the node being split is not the root, updating the parent nodes of v1 and v2
            v1->parent_node = v->parent_node;
            v2->parent_node = v->parent_node;

            // Finding the index of the original node v in its parent's children vector, removing it, and inserting v1 and v2 in its place, along with their corresponding MBRs
            for(int i=0; i<v->parent_node->children.size(); i++){
                if(v == v->parent_node->children[i]){
                    v->parent_node->children.erase(v->parent_node->children.begin() + i);
                    v->parent_node->children.insert(v->parent_node->children.begin() + i, {v1, v2});

                    v->parent_node->mbrs.erase(v->parent_node->mbrs.begin() + i);
                    v->parent_node->mbrs.insert(v->parent_node->mbrs.begin() + i, {mbr1, mbr2});
                    break;
                }
            }

            // If either of the new nodes is below the minimum node capacity, sharing MBRs and their corresponding child nodes with the sibling node to balance their sizes
            if(v1->children.size() < MIN_NODE_CAPACITY){
                Share(v1, v2);
            }else if(v2->children.size() < MIN_NODE_CAPACITY){
                Share(v2, v1);
            }
        }else{
            // If the node being split is the root, creating a new root node and assigning v1 and v2 as its children, updating their parent_node pointers accordingly
            Node *new_root = new Node(false);
            new_root->children = {v1, v2};
            new_root->mbrs = {mbr1, mbr2};
            this->root = new_root;
            v1->parent_node = new_root;
            v2->parent_node = new_root;
        }
    }

    // Deleting the original node v after the split operation to free up memory
    delete v;

    // If the parent node of v1 and v2 exceeds the maximum node capacity after the split, recursively splitting the parent node to maintain the R-tree structure properties
    if(v1->parent_node->children.size() > MAX_NODE_CAPACITY){
        Split(v1->parent_node);
    }
}


void RTree::Share(Node *v1, Node *v2){
    // The Share function is responsible for redistributing movies or MBRs between two sibling nodes (v1 and v2) to ensure that both nodes meet the minimum capacity requirements
    
    if(v1->is_leaf){
        // If the nodes are leaf nodes, the function redistributes movies between them

        // Finding the MBR of v1 in its parent's MBRs to use as a reference for calculating enlargements
        MBR mbr1, mbr2;
        for(int i=0; i<v1->parent_node->children.size(); i++){
            if(v1->parent_node->children[i] == v1){
                mbr1 = v1->parent_node->mbrs[i];
            }
        }

        // Calculating the enlargement required to add each movie from v2 to v1's MBR, storing these values in a vector
        vector<float> enlargements;
        for(int i=0; i<v2->movies.size(); i++){
            MBR temp_mbr(mbr1);
            temp_mbr.updateMBR(v2->movies[i]);
            enlargements.push_back(temp_mbr.getArea() - mbr1.getArea());
        }

        // While v1 has fewer movies than the minimum leaf capacity adding movies from v2 to v1
        while(v1->movies.size() < MIN_LEAF_CAPACITY){
            // Finding the index of the movie in v2 that requires the least enlargement to be added to v1's MBR
            int min_index = 0;
            float min_enlargement = enlargements[0];
            for(int i=1; i<enlargements.size(); i++){
                if(enlargements[i] < min_enlargement){
                    min_enlargement = enlargements[i];
                    min_index = i;
                }
            }

            // Adding the selected movie from v2 to v1, updating v1's MBR, and removing the movie from v2 and the corresponding enlargement value
            v1->movies.push_back(v2->movies[min_index]);
            mbr1.updateMBR(v2->movies[min_index]);
            v2->movies.erase(v2->movies.begin() + min_index);
            enlargements.erase(enlargements.begin() + min_index);
        }

        // After redistributing the movies, updating v2's MBR based on its remaining movies
        for(int i=0; i<v2->movies.size(); i++){
            mbr2.updateMBR(v2->movies[i]);
        }

        // Updating the MBRs in the parent node to reflect the new MBRs of v1 and v2
        for(int i=0; i<v1->parent_node->children.size(); i++){
            if(v1->parent_node->children[i] == v1){
                v1->parent_node->mbrs[i] = mbr1;
            }
            if(v1->parent_node->children[i] == v2){
                v1->parent_node->mbrs[i] = mbr2;
            }
        }
    }else{
        // If the nodes are not leaf nodes, the function redistributes MBRs and their corresponding child nodes between them

        // Finding the MBR of v1 in its parent's MBRs to use as a reference for calculating enlargements
        MBR mbr1, mbr2;
        for(int i=0; i<v1->parent_node->children.size(); i++){
            if(v1->parent_node->children[i] == v1){
                mbr1 = v1->parent_node->mbrs[i];
            }
        }

        // Calculating the enlargement required to add each MBR from v2 to v1's MBR, storing these values in a vector
        vector<float> enlargements;
        for(int i=0; i<v2->mbrs.size(); i++){
            MBR temp_mbr(mbr1);
            temp_mbr.updateMBR(v2->mbrs[i]);
            enlargements.push_back(temp_mbr.getArea() - mbr1.getArea());
        }

        // While v1 has fewer children than the minimum node capacity, adding MBRs and their corresponding child nodes from v2 to v1
        while(v1->children.size() < MIN_NODE_CAPACITY){
            // Finding the index of the MBR in v2 that requires the least enlargement to be added to v1's MBR
            int min_index = 0;
            float min_enlargement = enlargements[0];
            for(int i=1; i<enlargements.size(); i++){
                if(enlargements[i] < min_enlargement){
                    min_enlargement = enlargements[i];
                    min_index = i;
                }
            }

            // Adding the selected MBR and its corresponding child node from v2 to v1, updating v1's MBR, and removing the MBR and child node from v2 and the corresponding enlargement value
            v1->mbrs.push_back(v2->mbrs[min_index]);
            v1->children.push_back(v2->children[min_index]);
            v2->children[min_index]->parent_node = v1;
            mbr1.updateMBR(v2->mbrs[min_index]);
            v2->mbrs.erase(v2->mbrs.begin() + min_index);
            v2->children.erase(v2->children.begin() + min_index);
            enlargements.erase(enlargements.begin() + min_index);
        }

        // After redistributing the MBRs and child nodes, updating v2's MBR based on its remaining MBRs
        for(int i=0; i<v2->mbrs.size(); i++){
            mbr2.updateMBR(v2->mbrs[i]);
        }

        // Updating the MBRs in the parent node to reflect the new MBRs of v1 and v2
        for(int i=0; i<v1->parent_node->children.size(); i++){
            if(v1->parent_node->children[i] == v1){
                v1->parent_node->mbrs[i] = mbr1;
            }
            if(v1->parent_node->children[i] == v2){
                v1->parent_node->mbrs[i] = mbr2;
            }
        }
    }
}


void RTree::Merge(Node *v){
    // Two cases are handled separately: when the node v is a leaf and when it is not. 
    // This is because the pointer types and the data structures involved are different for leaf nodes and non-leaf nodes, requiring different update strategies.

    if(v->is_leaf){
        // Finding the index of the node v in its parent's children vector and storing its MBR for later use
        MBR mbr;
        int index = 0;
        for(int i=0; i<v->parent_node->children.size(); i++){
            if(v->parent_node->children[i] == v){
                mbr = MBR(v->parent_node->mbrs[i]);
                index = i;
                break;
            }
        }

        // Iterating through the sibling nodes of v to find a sibling that can accommodate the movies of v without exceeding the maximum leaf capacity
        for(int i=0; i<v->parent_node->children.size(); i++){
            if(v->parent_node->children[i] == v){
                continue;
            }

            if(v->parent_node->children[i]->movies.size() + v->movies.size() <= MAX_LEAF_CAPACITY){
                // When an appropriate sibling is found, merging the movies of the sibling node into v and updating the MBR
                Node *u = v->parent_node->children[i];
                
                for(int j=0; j<u->movies.size(); j++){
                    v->movies.push_back(u->movies[j]);
                    mbr.updateMBR(u->movies[j]);
                }
                v->parent_node->mbrs[index] = mbr;
                v->parent_node->children.erase(v->parent_node->children.begin() + i);
                v->parent_node->mbrs.erase(v->parent_node->mbrs.begin() + i);
                delete u;
                break;
            }
        }
    }else{
        // Finding the index of the node v in its parent's children vector and storing its MBR for later use
        MBR mbr;
        int index = 0;
        for(int i=0; i<v->parent_node->children.size(); i++){
            if(v->parent_node->children[i] == v){
                mbr = MBR(v->parent_node->mbrs[i]);
                index = i;
                break;
            }
        }

        // Iterating through the sibling nodes of v to find a sibling that can accommodate the children of v without exceeding the maximum node capacity
        for(int i=0; i<v->parent_node->children.size(); i++){
            if(v->parent_node->children[i] == v){
                continue;
            }
    
            if(v->parent_node->children[i]->children.size() + v->children.size() <= MAX_NODE_CAPACITY){
                // When an appropriate sibling is found, merging the children of the sibling node into v and updating the MBR
                Node *u = v->parent_node->children[i];

                for(int j=0; j<u->children.size(); j++){
                    v->children.push_back(u->children[j]);
                    v->mbrs.push_back(u->mbrs[j]);
                    v->children[v->children.size()-1]->parent_node = v;
                    mbr.updateMBR(u->mbrs[j]);
                }
                v->parent_node->mbrs[index] = mbr;
                v->parent_node->children.erase(v->parent_node->children.begin() + i);
                v->parent_node->mbrs.erase(v->parent_node->mbrs.begin() + i);
                delete u;
                break;
            }
        }
    }
}


Node* RTree::chooseSubtree(Node *v, Movie movie){
    // Choosing the child node of v that requires the minimum enlargement of its MBR to accommodate the given movie
    Node *best_child = nullptr;
    vector<float> enlargements;
    
    // Calculating the enlargement required for each child node's MBR to include the given movie and storing the values in a vector
    for(int i=0; i<v->children.size(); i++){
        MBR temp_mbr(v->mbrs[i]);
        temp_mbr.updateMBR(movie);
        enlargements.push_back(temp_mbr.getArea() - v->mbrs[i].getArea());
    }
    
    // Selecting the child node with the minimum enlargement value
    float min_enlargement = enlargements[0];
    best_child = v->children[0];
    for(int i=1; i<enlargements.size(); i++){
        if(enlargements[i] < min_enlargement){
            min_enlargement = enlargements[i];
            best_child = v->children[i];
        }
    }

    return best_child;
}


void RTree::printElements(Node *v){
    // Printing the elements of the R-tree to a CSV file, traversing the tree recursively and writing the movie data of the leaves to the output file
    if(v == this->root){
        fout1 << "id;title;runtime;vote_average;popularity;original_language;release_date;production_companies" << endl;
    }
    
    if(v->is_leaf){
        for(int i=0; i<v->movies.size(); i++){
            fout1 << v->movies[i].id << ";" << v->movies[i].title << ";" << v->movies[i].runtime << ";" << v->movies[i].vote_average << ";"<< v->movies[i].popularity << ";" << v->movies[i].original_language << ";" << v->movies[i].release_date.toString() << ";";
            fout1 << "[";
            for(int j=0; j<v->movies[i].production_companies.size(); j++){
                fout1 << "\'" << v->movies[i].production_companies[j] << "\'";
                if(j != v->movies[i].production_companies.size()-1){
                    fout1 << ",";
                }
            }
            fout1 << "]" << endl;
        }
        return;
    }
    
    for(int i=0; i<v->children.size(); i++){
        printElements(v->children[i]);
    }
}


void queryResultToCsv(vector<Movie> result){
    // Writing a query result to a CSV file
    fout2 << "id;title;runtime;vote_average;popularity;original_language;release_date;production_companies" << endl;
    for(int i=0; i<result.size(); i++){
        fout2 << result[i].id << ";" << result[i].title << ";" << result[i].runtime << ";" << result[i].vote_average << ";" << result[i].popularity << ";" << result[i].original_language << ";" << result[i].release_date.toString() << ";";
        fout2 << "[";
        for(int j=0; j<result[i].production_companies.size(); j++){
            fout2 << "\'" << result[i].production_companies[j] << "\'";
            if(j != result[i].production_companies.size()-1){
                fout2 << ",";
            }
        }
        fout2 << "]" << endl;
    }
}


int main(){
    ifstream fin("movies csv data\\data_movies_clean.csv");

    auto start = std::chrono::high_resolution_clock::now();
    RTree tree(fin);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    cout << "RTree Construction Time: " << duration.count() << " seconds" << std::endl;

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

    vector<Movie> result;
    MBR search_mbr(min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
    tree.RangeSearch(tree.root, result, search_mbr, original_languages);
    
    cout << "Query Result Size: " << result.size() << endl;
    queryResultToCsv(result);
}
