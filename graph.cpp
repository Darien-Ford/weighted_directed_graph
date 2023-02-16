#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <list>
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;


class Graph {
    private:
        int order;                          // Number of vertices in graph
        int size;                           // Number of arcs in graph
        vector<map<int, double>> graph;     // Vector containing vertices in graph

        // Add an arc between given vertices with given weight
        void arc(int from, int to, double weight) { graph[from][to] = weight; size++; };

    public:
        // Constructs the graph from a file
        Graph(const string& filename) : order(0), size(0) {

            ifstream file(filename);
            
            if (!file.is_open()) {
                cerr << filename << " CANNOT BE OPENED" << endl;
                exit(EXIT_FAILURE);
            }

            int from, to;
            float weight;
            map<int, double> vertice;
            
            string line;
            getline(file, line);
            istringstream ss(line);
            ss >> order;

            for (int i=0; i < order; i++) {
                graph.push_back(vertice);
            }

            while (getline(file, line)) {
                ss.str(line);
                ss >> from >> to >> weight;
                arc(from, to, weight);
            }
        };
        
        ~Graph() {};
        Graph(const Graph&) = delete;
        Graph& operator= (const Graph&) = delete;

        int getOrder() const { return order; };
        int getSize() const { return size; };

        // Get weight of arc between given vertices if arc exists
        double getArcWeight(int from, int to) const { return (graph[from].find(to) != graph[from].end()) ? graph[from].at(to) : numeric_limits<double>::max(); };
        
        // Determines if arc exists between given vertices
        bool connected(int from, int to) const { return (graph[from].find(to) != graph[from].end()) ? true : false; };
        
        bool pathExists(int start, int finish, vector<int> vertices) const;
        bool validPath(const list<int>& path) const;
        double pathWeight(const list<int>& path) const;
        list<int> minimumCostPath(int start, int finish) const;
};

// Determines if there is a path between given vertices
bool Graph::pathExists(int start, int finish, vector<int> vertices = {}) const {
    
    if (vertices.empty()) {
       
        for (int i = 0; i < order; i++) {
            vertices.push_back(0);
        }
        
        vertices[start] = 1;
    }
    
    if (start != finish) {

        for (map<int, double>::const_iterator it = graph[start].begin(); it != graph[start].end(); it++) {

            if (it->first == finish) { return true; }

            else if (it->first != finish && graph[it->first].size() != 0 && vertices[it->first] == 0) {
                vertices[it->first] = 1;

                if (pathExists(it->first, finish, vertices)) { return true; }
            }
        }

        return false;
    }

    return true;
}

// Determines if given path exists in graph
bool Graph::validPath(const list<int> & path) const {

    list<int>::const_iterator finish = path.begin();
    list<int>::const_iterator start = finish;
    finish++;

    while (finish != path.end()) {
        if (!connected(*start, *finish)) { return false; }
        start = finish;
        finish++;
    }

    return true;
}

// Calculates weight of given path
double Graph::pathWeight(const list<int>& path) const {

    double weight = 0;
    list<int>::const_iterator finish = path.begin();
    list<int>::const_iterator start = finish;
    finish++;

    while (finish != path.end()) {
        if (getArcWeight(*start, *finish) != numeric_limits<double>::max()) { weight += getArcWeight(*start, *finish); }
        else { return numeric_limits<double>::max(); }
        start = finish;
        finish++;
    }

    return weight;
}

// Finds the minimum cost path between given vertices
list<int> Graph::minimumCostPath(int start, int finish) const {

    list<int> path;
    vector<double> minimum;
    vector<int> vertices;
    set<pair<int, double>> queue;

    for (int i = 0; i < order; i++) {
        minimum.push_back(numeric_limits<double>::max());
    }

    minimum[start] = 0;

    for (int i = 0; i < order; i++) {
        vertices.push_back(-1);
    }

    queue.insert(make_pair(start, minimum[start]));

    if (start == finish) {
        path.push_front(start);
        return path;
    }

    while (!queue.empty()) {
        double dist = queue.begin()->second;
        int temp = queue.begin()->first;
        queue.erase(queue.begin());
        map<int,double> connections = graph[temp];

        for (map<int,double>::const_iterator it = connections.begin(); it != connections.end(); it++) {
            int vertex = it->first;
            double weight = it->second;
            double dist_total = dist + weight;

            if (dist_total < minimum[vertex]) {
                queue.erase(make_pair(vertex, minimum[vertex]));
                minimum[vertex] = dist_total;
                vertices[vertex] = temp;
                queue.insert(make_pair(vertex, minimum[vertex]));
            }
        }
    }

    for (int vertex = finish; vertex != -1; vertex = vertices[vertex]) {
        path.push_front(vertex);
    }

    return path;
}


int main(int argc, char* argv[]) {

    if (argc != 4) {
        cerr << "Incorrect number of command line arguments." << endl;
        cerr << "Usage: " << argv[0] << " <filename> <start vertex> <finish vertex>" << endl;
        exit(EXIT_FAILURE);
    }

    Graph graph(argv[1]);

    cout << "Graph Created: " << graph.getOrder() << " Vertices, " << graph.getSize() << " Arcs" << endl << endl;

    int start = atoi(argv[2]);
    int finish = atoi(argv[3]);

    if (graph.pathExists(start, finish)) {
        cout << "Minimum Cost Path from vertex " << start << " to vertex " << finish << ":" << endl;

        list<int> path = graph.minimumCostPath(start, finish);

        for (list<int>::iterator it = path.begin(); it != path.end(); it++) {
            if (it != path.begin()) { cout << " -> "; }
            cout <<  * it;
        }
        
        cout << endl;

        if (!graph.validPath(path)) {
            cerr << "Error: Path is not valid!" << endl;
        }
        else {
            cout << "Weight = " << graph.pathWeight(path) << endl;
        }
    }
    else {
        cout << "No path from " << start << " to " << finish << " exists!" << endl;
    }
    return 0;
}
