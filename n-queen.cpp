#include <bits/stdc++.h>
using namespace std;
const int N = 10010;
struct Base {
    int n;
    int row[N], col[N], queen[N];
    int pdiag[2 * N], cdiag[2 * N];
    Base() {
        n = 0;
        memset(row, 0, sizeof(row));
        memset(col, 0, sizeof(col));
        memset(pdiag, 0, sizeof(pdiag));
        memset(cdiag, 0, sizeof(cdiag));
        memset(queen, 0, sizeof(queen));
    }
    Base(const Base& obj) {
        n = obj.n;
        memcpy(row, obj.row, sizeof(row));
        memcpy(col, obj.col, sizeof(col));
        memcpy(pdiag, obj.pdiag, sizeof(pdiag));
        memcpy(cdiag, obj.cdiag, sizeof(cdiag));
        memcpy(queen, obj.queen, sizeof(queen));
    }
    Base operator=(const Base obj) {
        n = obj.n;
        memcpy(row, obj.row, sizeof(row));
        memcpy(col, obj.col, sizeof(col));
        memcpy(pdiag, obj.pdiag, sizeof(pdiag));
        memcpy(cdiag, obj.cdiag, sizeof(cdiag));
        memcpy(queen, obj.queen, sizeof(queen));
        return *this;
    }
    int getc(int x, int y) { return x + y - 1; }
    int getp(int x, int y) { return x - y + n; }
    void create_map() {
        bool vis[N] = { false };
        for(int i = 1; i <= n; ++i) {
            vector<int> tmp;
            for(int j = 1; j <= n; ++j) if(!vis[j]) tmp.push_back(j);
            int x = tmp[rand() % tmp.size()];
            queen[i] = x;
            vis[x] = true;
        }
        memset(row, 0, sizeof(row));
        memset(col, 0, sizeof(col));
        memset(pdiag, 0, sizeof(pdiag));
        memset(cdiag, 0, sizeof(cdiag));
        for(int i = 1; i <= n; ++i) {
            ++row[i];
            ++col[queen[i]];
            ++pdiag[getp(i, queen[i])];
            ++cdiag[getc(i, queen[i])];
        }
    }
    void show_map() {
        for(int j = 1; j <= 2 * n; ++j) {
            cout << "--";
        }
        cout << "-" << '\n';
        for(int i = 1; i <= n; ++i) {
            cout << "|";
            for(int j = 1; j <= n; ++j) {
                cout << (queen[i] == j ? " O |" : "   |");
            }
            cout << '\n';
            for(int j = 1; j <= 2 * n; ++j) {
                cout << "--";
            }
            cout << "-" << '\n';
        }
    }
    bool check() {
        for(int i = 1; i <= n; ++i) {
            if(col[queen[i]] != 1 || pdiag[getp(i, queen[i])] != 1 || cdiag[getc(i, queen[i])] != 1) return false;
        }
        return true;
    }
    bool adjust(int x) {
        int cur = queen[x];
        int best_cur = cur;
        int best_all = col[cur] + pdiag[getp(x, cur)] - 1 + cdiag[getc(x, cur)] - 1;
        for(int i = 1; i <= n; ++i) {
            if(i == cur) continue;
            int all = col[i] + pdiag[getp(x, i)] + cdiag[getc(x, i)];
            if(all < best_all) {
                best_cur = i;
                best_all = all;
            }
        }
        if(best_cur == cur) return false;
        --col[cur];
        --pdiag[getp(x, cur)];
        --cdiag[getc(x, cur)];
        ++col[best_cur];
        ++pdiag[getp(x, best_cur)];
        ++cdiag[getc(x, best_cur)];
        queen[x] = best_cur;
        return check();
    }
};
class Solve {
public:
    virtual Base* solve(int n);
};
class Hill_Climbing: public Solve {
public:
    Base* Map = new Base();
    ~Hill_Climbing() { delete Map; }
    Base* solve(int n) {
        Map->n = n;
        int T = n * n;
        while(T--) {
            Map->create_map();
            if(Map->check()) return Map;
            int cnt = 1000;
            while(cnt--) {
                for(int i = 1; i <= n; ++i) {
                    if(Map->adjust(i)) return Map;
                }
            }
        }
        return NULL;
    }
} hill_climbing;
class CSP: public Solve {
public:
    Base* Map = new Base();
    ~CSP() { delete Map; }
    Base* solve(int n) {
        Map->n = n;
        Map->create_map();
        if(Map->check()) return Map;
        int T = n * n;
        while(T--) {
            for(int i = 1; i <= n; ++i) {
                if(Map->adjust(i)) return Map;
            }
        }
        return NULL;
    }
} csp;
class Inheritance: public Solve {
public:
    const static int population_size = 30;
    const int iteration = 1000;
    const double crossover_probability = 0.8;
    const double mutation_probability = 1;
    int n;
    Base Population[population_size + 1];
    void init(int n) {
        for(int i = 1; i <= population_size; ++i) {
            Population[i].n = n;
            Population[i].create_map();
        }
        sort(Population + 1, Population + population_size + 1, [&](Base& x, Base& y) { return get_weight(x) < get_weight(y); });
    }
    int get_weight(Base& population) {
        int res = 0;
        for(int i = 1; i <= n; ++i) {
            int cur = population.queen[i];
            res += population.col[cur] + population.pdiag[population.getp(i, cur)] + population.cdiag[population.getc(i, cur)] - 3;
        }
        return res / 2;
    }
    int choose_population() {
        int sum = 0;
        for(int i = 1; i <= population_size; ++i) {
            sum += get_weight(Population[i]);
        }
        double Rand = rand() * 1.0 / RAND_MAX;
        double tmp = 0.0;
        int ttmp = 0;
        for(int i = 1; i <= population_size; ++i) {
            ttmp += get_weight(Population[i]);
            tmp += (sum - ttmp) * 1.0 / sum;
            if(tmp >= Rand) return i;
        }
        return 1;
    }
    void swap_queen(Base& population1, Base& population2, int index) {
        --population1.col[population1.queen[index]];
        --population2.col[population2.queen[index]];
        --population1.cdiag[population1.getc(index, population1.queen[index])];
        --population2.cdiag[population2.getc(index, population2.queen[index])];
        --population1.pdiag[population1.getp(index, population1.queen[index])];
        --population2.pdiag[population2.getp(index, population2.queen[index])];
        swap(population1.queen[index], population2.queen[index]);
        ++population1.col[population1.queen[index]];
        ++population2.col[population2.queen[index]];
        ++population1.cdiag[population1.getc(index, population1.queen[index])];
        ++population2.cdiag[population2.getc(index, population2.queen[index])];
        ++population1.pdiag[population1.getp(index, population1.queen[index])];
        ++population2.pdiag[population2.getp(index, population2.queen[index])];
    }
    void cross(int index1, int index2) {
        int index = rand() % n + 1;
        while(true) {
            int tag = -1;
            swap_queen(Population[index1], Population[index2], index);
            for(int i = 1; i <= n; ++i) {
                if(i != index && Population[index1].queen[i] == Population[index1].queen[index]) {
                    tag = i;
                    break;
                }
            }
            if(tag == -1) break;
            index = tag;
        }
    }
    void mutate(int index) {
        int index1 = rand() % n + 1;
        int index2 = rand() % n + 1;
        while(index1 == index2) index2 = rand() % n + 1;
        Base& population = Population[index];
        --population.col[population.queen[index1]];
        --population.col[population.queen[index2]];
        --population.cdiag[population.getc(index1, population.queen[index1])];
        --population.cdiag[population.getc(index2, population.queen[index2])];
        --population.pdiag[population.getp(index1, population.queen[index1])];
        --population.pdiag[population.getp(index2, population.queen[index2])];
        swap(population.queen[index1], population.queen[index2]);
        ++population.col[population.queen[index1]];
        ++population.col[population.queen[index2]];
        ++population.cdiag[population.getc(index1, population.queen[index1])];
        ++population.cdiag[population.getc(index2, population.queen[index2])];
        ++population.pdiag[population.getp(index1, population.queen[index1])];
        ++population.pdiag[population.getp(index2, population.queen[index2])];
    }
    Base tmp[population_size + 1];
    void evolution() {
        sort(Population + 1, Population + population_size + 1, [&](Base& x, Base& y) { return get_weight(x) < get_weight(y); });
        for(int i = 1; i <= population_size; ++i) tmp[i] = Population[choose_population()];
        for(int i = 1; i <= population_size; ++i) Population[i] = tmp[i];
        sort(Population + 1, Population + population_size + 1, [&](Base& x, Base& y) { return get_weight(x) < get_weight(y); });
    }
    Base* solve(int n) {
        this->n = n;
        init(n);
        int T = iteration;
        while(T--) {
            if(Population[1].check()) return &Population[1];
            for(int i = 1; i <= population_size; i += 2) {
                if(i + 1 <= population_size && rand() * 1.0 / RAND_MAX <= crossover_probability) {
                    cross(i, i + 1);
                }
                else {
                    if(i <= population_size && rand() * 1.0 / RAND_MAX <= mutation_probability) {
                        mutate(i);
                    }
                    if(i + 1 <= population_size && rand() * 1.0 / RAND_MAX <= mutation_probability) {
                        mutate(i + 1);
                    }
                }
            }
            evolution();
        }
        return NULL;
    }
} inheritance;
void show() {
    cout << "Please choose a algorithm to solve your n-queens problem." << '\n';
    cout << "1. hill_climbing" << '\n';
    cout << "2. csp" << '\n';
    cout << "3. inheritance" << '\n';
    cout << "Input your choice here: ";
}
void solve(Solve* sol) {
    cout << "Please input an integer n as the size of the map: ";
    int n;
    cin >> n;
    clock_t Start = clock();
    Base* res = sol->solve(n);
    clock_t End = clock();
    cout << "The total cost time is " << (End - Start) << "ms" << '\n';
    if(res == NULL) {
        cout << "No find!" << '\n';
        system("pause");
    }
    else {
        char c;
        while(true) {
            cout << "Successfully find!" << '\n';
            cout << "If you want to show the map?(y/n): ";
            cin >> c;
            if(c != 'y' && c != 'n') {
                cout << "Invalid input!" << '\n';
                continue;
            }
            else break;
        }
        if(c == 'y') {
            res->show_map();
            system("pause");
        }
    }
}
int main() {
    srand((unsigned int)time(NULL));
    while(true) {
        show();
        int op;
        cin >> op;
        switch(op) {
        case 1: solve(&hill_climbing); break;
        case 2: solve(&csp); break;
        case 3: solve(&inheritance); break;
        default: break;
        }
        system("cls");
    }
    return 0;
}
