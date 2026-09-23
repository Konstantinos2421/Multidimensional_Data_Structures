#include "utilities.cpp"

// Defining constants for the LSH process
#define SHINGLE_SIZE 3
#define PERMUTATIONS_NUMBER 300
#define ROWS_PER_BAND 5
#define BANDS (PERMUTATIONS_NUMBER/ROWS_PER_BAND)

using namespace std;
ofstream fout("results\\LSH_similarity_result.txt");

class LSH{
    public:
        // Attributed needed for the LSH process
        vector<string> production_companies;
        vector<string> shingles;
        vector< vector<bool> > shingle_in_production_company;
        vector< vector<int> > signatures;
        unordered_map< size_t, vector<int> > buckets;

    LSH(vector<string> production_companies){
        this->production_companies = production_companies;
        companiesToShingles();
        minHash();
        hashToBuckets();
    }

    void companiesToShingles(){
        // Extracting shingles of size SHINGLE_SIZE from each production company name and storing them in the shingles vector, ensuring uniqueness
        shingles.clear();
        for(int i=0; i<production_companies.size(); i++){
            for(int j=0; j+SHINGLE_SIZE<=production_companies[i].size(); j++){
                string shingle = production_companies[i].substr(j, SHINGLE_SIZE);
                if(find(shingles.begin(), shingles.end(), shingle) == shingles.end()){
                    shingles.push_back(shingle);
                }
            }
        }
    }

    void minHash(){
        // Creating a boolean matrix indicating which shingles are present in each production company name
        shingle_in_production_company.resize(shingles.size());
        for(int i=0; i<shingle_in_production_company.size(); i++){
            shingle_in_production_company[i].resize(production_companies.size(), false);
        }
        
        // Initializing the boolean matrix by checking for the presence of each shingle in each production company name
        for(int i=0; i<shingles.size(); i++){
            for(int j=0; j<production_companies.size(); j++){
                if(production_companies[j].find(shingles[i]) != string::npos){
                    shingle_in_production_company[i][j] = true;
                }
            }
        }
        
        // Generating random coefficients for the hash functions used in the MinHash process, ensuring that they are within the range of the number of shingles
        vector<int> a(PERMUTATIONS_NUMBER), b(PERMUTATIONS_NUMBER);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> distA(1, shingles.size()-1);
        uniform_int_distribution<int> distB(0, shingles.size()-1);
        a[0] = 1;
        b[0] = 0;
        for(int i=1; i<PERMUTATIONS_NUMBER; i++){
            a[i] = distA(gen);
            b[i] = distB(gen);
        }

        // Initializing the signatures matrix with maximum integer values
        signatures.resize(PERMUTATIONS_NUMBER);
        for(int i=0; i<signatures.size(); i++){
            signatures[i].resize(production_companies.size(), MAX_INT);
        }

        // Calculating the MinHash signatures for each production company by applying the hash functions to the shingles and updating the signatures matrix with the minimum hash values
        for(int i=0; i<PERMUTATIONS_NUMBER; i++){
            for(int j=0; j<shingles.size(); j++){
                int hi = (a[i]*j + b[i]) % shingles.size();
                for(int k=0; k<production_companies.size(); k++){
                    if(shingle_in_production_company[j][k]){
                        signatures[i][k] = min(signatures[i][k], hi);
                    }
                }
            }
        }
    }

    void hashToBuckets(){
        // Hashing the MinHash signatures into buckets for each band
        hash<string> hasher;
        for(int i=0; i<BANDS; i++){
            // For each band, creating a unique signature string for each production company
            for(int j=0; j<production_companies.size(); j++){
                // Creating a vector to hold the signature values for the current band and production company
                vector<int> band_sign;
                for(int r=0; r<ROWS_PER_BAND; r++){
                    band_sign.push_back(signatures[i*ROWS_PER_BAND + r][j]);
                }

                // Creating a string representation of the band signature to use as a key for hashing into buckets
                string signature_str;
                for(int k=0; k<band_sign.size(); k++){
                    signature_str += band_sign[k];
                    if(k != band_sign.size()-2){
                        signature_str += "|";
                    }
                }

                // Hashing the signature string to determine the bucket ID and adding the production company index to the corresponding bucket if it is not already present
                size_t bucket_id = hasher(signature_str);
                if(find(buckets[bucket_id].begin(), buckets[bucket_id].end(), j) == buckets[bucket_id].end()){
                    buckets[bucket_id].push_back(j);
                }
            }
        }
    }

    vector<string> mostSimilarCompanies(int n){
        // If the requested number of similar companies is greater than or equal to the total number of production companies, return all production companies
        if(n >= production_companies.size()){
            return production_companies;
        }

        vector< pair<int, int> > candidate_pairs;                           // Vector to hold pairs of production company indices that are candidates for similarity comparison
        vector<double> best_similarity(production_companies.size(), 0.0);   // Vector to hold the best similarity scores for each production company
        vector<int> topN;                                                   // Vector to hold the indices of the top N most similar production companies

        // Iterating through the buckets to find candidate pairs of production companies that have been hashed into the same bucket, indicating potential similarity
        for(pair<size_t,vector<int>> bucket : buckets){
            if(bucket.second.size() > 1){
                for(int i=0; i<bucket.second.size()-1; i++){
                    for(int j=i+1; j<bucket.second.size(); j++){
                        if(find(candidate_pairs.begin(), candidate_pairs.end(), make_pair(i,j)) == candidate_pairs.end() &&
                           find(candidate_pairs.begin(), candidate_pairs.end(), make_pair(j,i)) == candidate_pairs.end()){
                            candidate_pairs.push_back({i,j});
                        }
                    }
                }
            }
        }

        // Calculating the Jaccard similarity for each candidate pair of production companies based on their MinHash signatures and updating the best similarity scores for each production company
        for(int i=0; i<candidate_pairs.size(); i++){
            double sim = 0;
            for(int j=0; j<signatures.size(); j++){
                if(signatures[j][candidate_pairs[i].first] == signatures[j][candidate_pairs[i].second]){
                    sim++;
                }
            }
            sim /= signatures.size();
            best_similarity[candidate_pairs[i].first] = max(best_similarity[candidate_pairs[i].first], sim);
            best_similarity[candidate_pairs[i].second] = max(best_similarity[candidate_pairs[i].second], sim);
        }

        // Finding the top N production companies with the highest similarity scores and storing their indices in the topN vector
        for(int i=0; i<production_companies.size(); i++){
            if(topN.size() < n){
                topN.push_back(i);
            }else{
                vector<int>::iterator min_it = min_element(topN.begin(), topN.end(), [&best_similarity](int a, int b){
                    return best_similarity[a] <= best_similarity[b];
                });

                if(best_similarity[i] > best_similarity[*min_it]){
                    topN.erase(min_it);
                    topN.push_back(i);
                }
            }
        }

        // Returning the names of the top N most similar production companies based on their indices stored in the topN vector
        vector<string> topNCompanies;
        for(int i=0; i<topN.size(); i++){
            topNCompanies.push_back(production_companies[topN[i]]);
        }
        return topNCompanies;
    }
};


void extractProductionCompanies(vector<Movie> movies, vector<string> &production_companies){
    // Extracting unique production company names from the list of movies and storing them in the production_companies vector
    for(int i=0; i<movies.size(); i++){
        for(int j=0; j<movies[i].production_companies.size(); j++){
            if(find(production_companies.begin(), production_companies.end(), movies[i].production_companies[j]) == production_companies.end()){
                production_companies.push_back(movies[i].production_companies[j]);
            }
        }
    }
}


int main(){
    ifstream fin("results\\query_result.csv");
    vector<Movie> movies;
    readCsvResult(fin, movies);

    vector<string> production_companies;
    extractProductionCompanies(movies, production_companies);

    auto start = std::chrono::high_resolution_clock::now();
    LSH lsh(production_companies);
    vector<string> topNCompanies = lsh.mostSimilarCompanies(15);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    cout << "LSH Duration: " << duration.count() << " seconds" << std::endl;
    
    for(int i=0; i<topNCompanies.size(); i++){
        fout << topNCompanies[i] << endl;
    }
}
