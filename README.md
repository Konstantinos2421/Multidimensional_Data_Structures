# Multidimensional Data Structures
A project that implements KDTrees, Quad Trees and R-Trees as well as LSH for 5D data using C++. This dataset contains a cleaned and structured collection of movie metadata sourced from The Movie Database(TMDB), covering films released between 1900 and 2025. The dataset can be found also in the link below:

[Kaggle Movies Dataset](https://www.kaggle.com/datasets/mustafasayed1181/movies-metadata-cleaned-dataset-19002025)

### Project Files:
- **movies csv data:** A folder that contains the csv file with the dataset and a readme file that explains it.
- **results:** A folder that contains the results that are returned from executing the code.
- **utilities.cpp:** This file implements general classes and functions that are useful for the implementation of all the other data structures.
- **KDTree.cpp:** The source code for implementing KDTrees.
- **QuadTree.cpp:** The source code for implementing Quad Trees.
- **Rtree.cpp:** The source code for implementing R-Trees.
- **LSH.cpp** The source code for implementing Locality Sensitive Hashing.


### Executing Instructions:
Essentially the purpose of the project is to execute queries like:

`Detect the N-top most similar Production-Company-Names\
of Movies\
with runtime from 30 up to 60 minutes,\
vote-average from 3 up to 5,\
took popularity from 3 up to 6,\
the origin-language is ‘US’ or ‘GB’\
and released during 2000 up to 2020`

Firstly, one of the following files have to be executed:
- KDTree.cpp
- QuadTree.cpp
- RTree.cpp

In these files the building of the corresponding data structure is done and then a range query is executed. The range is defined in the main function of each file. The execution of these files returns the result of the range query in the query_result.csv file in the results folder. Next, the file LSH.cpp have to be executed in order to execute the similarity query 
