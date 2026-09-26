# Multidimensional Data Structures

This project provides C++ implementations of multidimensional data structures —specifically **KD-Trees**, **Quadtrees**, and **R-Trees**— alongside **Locality-Sensitive Hashing (LSH)** for 5D data. The dataset consists of a cleaned and structured collection of movie metadata sourced from **The Movie Database (TMDB)**, covering films released between 1900 and 2025. 

The original dataset is available on Kaggle:  
[Kaggle Movies Dataset](https://www.kaggle.com/datasets/mustafasayed1181/movies-metadata-cleaned-dataset-19002025)

## Project Structure

- **/movies csv data**: Folder containing the dataset (`.csv`) along with a dedicated README file explaining its schema.
- **/results**: Folder where output files generated during execution are stored.
- **utilities.cpp**: Helper classes and utility functions shared across all data structure implementations.
- **KDTree.cpp**: Implementation and execution entry point for KD-Trees.
- **QuadTree.cpp**: Implementation and execution entry point for Quadtrees.
- **Rtree.cpp**: Implementation and execution entry point for R-Trees.
- **LSH.cpp**: Implementation of Locality-Sensitive Hashing (LSH) for similarity query.

## Execution Instructions

The main objective of this project is to perform complex range filtered similarity queries, such as:

> *Find the Top-N most similar production companies for movies that meet the following criteria:*\
> *Runtime: 30 to 60 minutes*\
> *Vote Average: 3 to 5*\
> *Popularity: 3 to 6*\
> *Original Language: 'US' or 'GB'*\
> *Release Year: 2000 to 2020"*

### Steps to Run the Project:

1. **Execute one of the Data Structures Indexing files:**
   - `KDTree.cpp`
   - `QuadTree.cpp`
   - `Rtree.cpp`

   Running any of these files builds the respective data structure and executes a range query over the 5D dataset. The search ranges can be modified directly within the `main()` function of each file. 
   
   The filtered subset of movies resulting from the range query will be saved to `results/query_result.csv`.

2. **Execute LSH for Similarity Querying:**

   Run `LSH.cpp` to perform the Locality-Sensitive Hashing similarity search on the movies retrieved in Step 1. This step computes and outputs the top-N most similar production companies. The parameter `N` can be configured within the `main()` function of `LSH.cpp`. The result of this execution will be saved to `results/LSH_similarity_result.txt`.
