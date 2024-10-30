#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

struct Point {
    double x, y;

    // overload operatorów dla struktury Points
    bool operator<(const Point& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }

    bool operator==(const Point& other) const {
        return (x == other.x) && (y == other.y);
    }

    bool operator!=(const Point& other) const {
        return (x != other.x) || (y != other.y);
    }
};

// funkcja zwracająca cross product dla 3 punktów, wynik mówi nam w jakiej relacji są 3 punkty (w linii, w rotacji zegarowej czy odwrotnej do zegarowej)
double crossProduct(const Point& p1, const Point& p2, const Point& p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

// fukcja zwracająca odległość 2 punktów od siebie
double distance(const Point& p1, const Point& p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// funkcja printująca rozwiązanie
void printHull(const std::vector<Point>& hull) {
    cout << "[";
    for (size_t i=0; i < hull.size(); i++) {
        cout << "(" << hull[i].x << ", " << hull[i].y << ")";
        if (i < hull.size() - 1) {
            cout << ", ";
        }
    }
    cout << "]\n";
}

// główna funkcja generująca rozwiązanie
vector<Point> grahamScan(vector<Point>& points) {
    // znajdujemy najmniejszy element względem kryteriów x, y
    Point p0 = *min_element(points.begin(), points.end());

    // umieszczamy najmniejszy element na pierwszym miejscu w wektorze
    if (p0 != points[0]) {
        for (size_t i = 1; i < points.size(); i++) {
            if (points[i] == p0) {
                iter_swap(points.begin(), points.begin() + i);
                break;
            }
        }
    }

    // sortujemy punkty według kątu względem pierwszego punktu, jeżeli kąt jest ten sam to dalszy punkt dodajemy na najpierw do wektora
    sort(points.begin() + 1, points.end(), [&](const Point& p1, const Point& p2) {
        double cross = crossProduct(p0, p1, p2);
        if (cross == 0) {
            return distance(p0, p1) > distance(p0, p2);
        }
        return cross > 0;
    });

    vector<Point> hull;
    bool initial_line = true;

    for (const auto& p : points) {
        // najpierw dodajemy do wyniku wszystkie punkty z najmniejszym kątem względem pierwszego punktu
        if (initial_line) {
            if (hull.size() < 2) {
                hull.push_back(p);
            }
            else if (hull.size() >= 2 && crossProduct(hull[hull.size() - 2], hull.back(), p) == 0)  {
                hull.insert(hull.begin() + 1, p);
            }
            else {
                initial_line = false;
                hull.push_back(p);
            }
        }
        else {
            // jeżeli cross product dla punktu pierwszego, poprzedniego i aktualnego jest ujemny (czyli punkty są w relacji zegarowej) to wyrzucamy poprzedniu punkt z rozwiązania i powtarzamy procedurę do otrzymania dodatniego cross productu
            while (hull.size() >= 2 && crossProduct(hull[hull.size() - 2], hull.back(), p) < 0) {
                hull.pop_back();
            }
            // dodajemy aktualny punkt do rozwiązania
            hull.push_back(p);
        }
    }

    return hull;
}

int main() {
    // wczytujemy dane z pliku
    string filename;
    cout << "Enter the filename: ";
    cin >> filename;

    ifstream file(filename);

    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        return 1;
    }

    int numPoints;
    file >> numPoints;

    vector<Point> points;
    for (int i = 0; i < numPoints; i++) {
        double x, y;
        file >> x >> y;
        points.push_back({x, y});
    }

    file.close();

    vector<Point> hull = grahamScan(points);

    printHull(hull);

    return 0;
}