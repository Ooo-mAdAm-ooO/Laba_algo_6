#include <iostream>
#include <vector>
#include <random>
#include <iomanip>

using namespace std;

class Graph {
public:
    Graph() = default;
    Graph(int n, double probability) { genAdjMatrix(n, probability); }

    // Генерация матрицы смежности
    void genAdjMatrix(int n, double probability) {
        vertexCount = n;
        matrix.assign(n, vector<int>(n, 0));
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> prob(0.0, 1.0);

        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
                if (prob(gen) < probability)
                    matrix[i][j] = matrix[j][i] = 1;

        countEdges();
        genAdjList();
    }

    // Вывод графа
    void print() const {
        cout << "\n=== Матрица смежности ===\n   ";
        for (int j = 0; j < vertexCount; j++) cout << setw(2) << j << " ";
        cout << "\n";
        for (int i = 0; i < vertexCount; i++) {
            cout << setw(2) << i << " ";
            for (int j = 0; j < vertexCount; j++)
                cout << setw(2) << matrix[i][j] << " ";
            cout << "\n";
        }

        cout << "\n=== Список смежности ===\n";
        for (int i = 0; i < vertexCount; i++) {
            cout << setw(2) << i << ": ";
            if (adjList[i].empty()) cout << "пусто";
            else for (int j : adjList[i]) cout << j << "(" << matrix[i][j] << ") ";
            cout << "\n";
        }
        cout << "Вершин: " << vertexCount << ", Рёбер: " << edgeCount << "\n";
    }

    // Отождествление вершин
    void otojdestvenie(int v1, int v2) {
        if (v1 < 0 || v2 < 0 || v1 >= vertexCount || v2 >= vertexCount || v1 == v2) {
            cout << "Ошибка: некорректные вершины.\n"; return;
        }

        vector<vector<int>> newMatrix(vertexCount - 1, vector<int>(vertexCount - 1, 0));
        vector<int> merged(vertexCount, 0);
        for (int i = 0; i < vertexCount; i++)
            merged[i] = (matrix[v1][i] || matrix[v2][i]) ? 1 : 0;

        int ni = 0;
        for (int i = 0; i < vertexCount; i++) {
            if (i == v2) continue;
            int nj = 0;
            for (int j = 0; j < vertexCount; j++) {
                if (j == v2) continue;
                newMatrix[ni][nj] = (i == v1) ? merged[j] : (j == v1 ? merged[i] : matrix[i][j]);
                nj++;
            }
            ni++;
        }

        matrix = move(newMatrix);
        vertexCount--;
        countEdges();
        genAdjList();
    }

    // Стягивание ребра
    void stjagivanie(int v1, int v2) {
        if (v1 < 0 || v2 < 0 || v1 >= vertexCount || v2 >= vertexCount || v1 == v2) {
            cout << "Ошибка: некорректные вершины.\n"; return;
        }
        if (!matrix[v1][v2]) {
            cout << "Ошибка: между вершинами " << v1 << " и " << v2 << " нет ребра.\n"; return;
        }
        otojdestvenie(v1, v2);
    }

    // Расщепление вершины
    void rassheplenie(int v) {
        if (v < 0 || v >= vertexCount) {
            cout << "Ошибка: некорректная вершина.\n"; return;
        }

        vector<vector<int>> newMatrix(vertexCount + 1, vector<int>(vertexCount + 1, 0));
        for (int i = 0; i < vertexCount; i++)
            for (int j = 0; j < vertexCount; j++)
                newMatrix[i][j] = matrix[i][j];

        int newV = vertexCount;
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> dist(0.0, 1.0);

        for (int j = 0; j < vertexCount; j++) {
            if (matrix[v][j] && j != v && dist(gen) < 0.5) {
                newMatrix[v][j] = newMatrix[j][v] = 0;
                newMatrix[newV][j] = newMatrix[j][newV] = 1;
            }
        }

        matrix = move(newMatrix);
        vertexCount++;
        countEdges();
        genAdjList();
    }

    // Объединение, пересечение, XOR
    static Graph unite(const Graph& g1, const Graph& g2) {
        return combine(g1, g2, [](int a, int b) { return a || b; });
    }
    static Graph intersection(const Graph& g1, const Graph& g2) {
        return combine(g1, g2, [](int a, int b) { return a && b; });
    }
    static Graph xorSum(const Graph& g1, const Graph& g2) {
        return combine(g1, g2, [](int a, int b) { return a != b; });
    }

    // Декартово произведение
    static Graph dekart(const Graph& g1, const Graph& g2) {
        int n1 = g1.vertexCount, n2 = g2.vertexCount;
        Graph g;
        g.vertexCount = n1 * n2;
        g.matrix.assign(g.vertexCount, vector<int>(g.vertexCount, 0));

        for (int u1 = 0; u1 < n1; u1++)
            for (int u2 = 0; u2 < n2; u2++)
                for (int v1 = 0; v1 < n1; v1++)
                    for (int v2 = 0; v2 < n2; v2++)
                        if ((u1 == v1 && g2.matrix[u2][v2]) || (u2 == v2 && g1.matrix[u1][v1]))
                            g.matrix[u1 * n2 + u2][v1 * n2 + v2] = 1;

        g.countEdges();
        g.genAdjList();
        return g;
    }

private:
    vector<vector<int>> matrix;
    vector<vector<int>> adjList;
    int vertexCount = 0;
    int edgeCount = 0;

    // Генерация списка смежности из матрицы
    void genAdjList() {
        adjList.assign(vertexCount, {});
        for (int i = 0; i < vertexCount; i++) {
            for (int j = 0; j < vertexCount; j++) {
                if (matrix[i][j] != 0) adjList[i].push_back(j);
            }
        }
    }

    // Подсчёт рёбер
    void countEdges() {
        edgeCount = 0;
        for (int i = 0; i < vertexCount; i++)
            for (int j = i + 1; j < vertexCount; j++)
                if (matrix[i][j]) edgeCount++;
    }

    // Универсальная операция над двумя графами
    static Graph combine(const Graph& g1, const Graph& g2, auto op) {
        int n = min(g1.vertexCount, g2.vertexCount);
        Graph g;
        g.vertexCount = n;
        g.matrix.assign(n, vector<int>(n, 0));
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                g.matrix[i][j] = op(g1.matrix[i][j], g2.matrix[i][j]);
        g.countEdges();
        g.genAdjList();
        return g;
    }

};


enum MainMenu {
    GEN = 1, TRANSFORM, OPERATIONS, DEKART, SHOW, EXIT = 0
};

void printMainMenu() {
    cout << "1. Генерация графов\n"
        << "2. Преобразования графа\n"
        << "3. Операции над двумя графами\n"
        << "4. Декартово произведение\n"
        << "5. Показать граф\n"
        << "0. Выход\n"
        << "Выберите пункт: ";
}

int selectGraph() {
    int g;
    cout << "С каким графом работать (1 или 2): ";
    cin >> g;
    if (g < 1 || g > 2) {
        cout << "Ошибка: нет такого графа.\n";
        return -1;
    }
    return g - 1;
}

void generateGraph(vector<Graph>& graphs) {
    while (true) {
        cout << "\nГенерация графа\n"
            << "1. Создать G1\n"
            << "2. Создать G2\n"
            << "0. Назад\n"
            << "Ваш выбор: ";
        int a; cin >> a;
        if (a == 0) break;

        int n; double p;
        cout << "Введите количество вершин: "; cin >> n;
        cout << "Введите вероятность ребра (0..1): "; cin >> p;

        int index = (a == 1 ? 0 : 1);
        graphs[index].genAdjMatrix(n, p);
        cout << "Граф G" << (index + 1) << " создан.\n";
        graphs[index].print();
    }
}

void transformGraph(vector<Graph>& graphs) {
    while (true) {
        cout << "\nПреобразования графа\n"
            << "1. Отождествление вершин\n"
            << "2. Стягивание ребра\n"
            << "3. Расщепление вершины\n"
            << "0. Назад\n"
            << "Ваш выбор: ";
        int a; cin >> a;
        if (a == 0) break;

        int idx = selectGraph();
        if (idx == -1) continue;

        int v1, v2, v;
        switch (a) {
        case 1:
            cout << "Введите вершины v1 v2: ";
            cin >> v1 >> v2;
            graphs[idx].otojdestvenie(v1, v2);
            break;
        case 2:
            cout << "Введите вершины v1 v2: ";
            cin >> v1 >> v2;
            graphs[idx].stjagivanie(v1, v2);
            break;
        case 3:
            cout << "Введите вершину: ";
            cin >> v;
            graphs[idx].rassheplenie(v);
            break;
        default: cout << "Неверный выбор.\n";
        }
        graphs[idx].print();
    }
}

void operateGraphs(vector<Graph>& graphs) {
    while (true) {
        cout << "\nОперации над графами\n"
            << "1. Объединение (G1 ∪ G2)\n"
            << "2. Пересечение (G1 ∩ G2)\n"
            << "3. Кольцевая сумма (G1 ⊕ G2)\n"
            << "0. Назад\n"
            << "Ваш выбор: ";
        int a; cin >> a;
        if (a == 0) break;

        switch (a) {
        case 1: graphs[2] = Graph::unite(graphs[0], graphs[1]); break;
        case 2: graphs[2] = Graph::intersection(graphs[0], graphs[1]); break;
        case 3: graphs[2] = Graph::xorSum(graphs[0], graphs[1]); break;
        default: cout << "Неверный выбор.\n"; continue;
        }
        cout << "Результат (G3):\n";
        graphs[2].print();
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    vector<Graph> graphs(3);
    int choice;

    while (true) {
        printMainMenu();
        cin >> choice;

        switch (choice) {
        case GEN:        generateGraph(graphs); break;
        case TRANSFORM:  transformGraph(graphs); break;
        case OPERATIONS: operateGraphs(graphs); break;
        case DEKART:
            graphs[2] = Graph::dekart(graphs[0], graphs[1]);
            cout << "Декартово произведение (G3):\n";
            graphs[2].print();
            break;
        case SHOW: {
            int i;
            cout << "Показать граф (1, 2 или 3): ";
            cin >> i;
            if (i >= 1 && i <= 3) graphs[i - 1].print();
            else cout << "Нет такого графа.\n";
            break;
        }
        case EXIT: cout << "Выход...\n"; return 0;
        default: cout << "Неверный выбор.\n";
        }
    }

    return 0;
}