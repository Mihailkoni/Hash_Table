#pragma once

#include <iostream> // база
#include <vector> // для массива
#include <fstream> // для чтения и записи файла
#include <string> // для работы со строками
using namespace std;

enum Node_status { // статусы ключа
    ZERO, ONE, TWO
};

struct Node {
    int line_number; // для номера строки

    Node_status status; // статус ячейки

    struct group { // Номер группы
        char l; // буква номера группы
        int num; // число номера группы
    }group;

    struct fio { // структура ФИО
        string fio; // ФИО
    }fio;

    struct Direction { // структура направления
        string title; // название направления
    }dir;

    struct Application { // структура номера заявки
        int number_app; // номер заявки
    }app;

    Node() : line_number(0), status(ZERO) {}
};


class Hash_Table {
private:
    const int k1 = 1; // константа для квадратичного поиска
    const int k2 = 2; // константа для квадратичного поиска
    vector<Node> table; // хэш-таблица
    int table_size; // текущее количество элементов в таблице
    int max_table_size; // ёмкость таблицы

    int hash_middle_square(const string fio, int num) { // хэш функция середина квадрата
        int sum = 0;
        for (char c : fio) { // суммируем символы фио
            sum += c;
        }
        sum += num; // прибавляем номер заявки
        long long square = sum * sum; // возводим в квадрат
        string square_str = to_string(square); // переводим в строку
        int squared_len = square_str.length(); // длина квадрата ключа
        int key_len = to_string(sum).length();; // длина ключа
        int mid_len = key_len / 2; // длина середины
        int start_mid_pos = (squared_len / 2) - mid_len / 2; // начало середины
        string mid_str = square_str.substr(start_mid_pos, key_len); // середина
        int hash = stoi(mid_str); // преобразуем в число           
        return hash % max_table_size;
    }

    int quadratic_search(int index, int i) { // квадратичный поиск места вставки
        return (index + (i * k1) + (k2 * i * i)) % max_table_size;
    }

    void rehash() {
        vector<Node> old_Table = table;
        int old_max_table_size = max_table_size;

        max_table_size = max_table_size * 2; // Увеличиваем ёмкость в 2 раза
        table.assign(max_table_size, Node()); // чистим и увеличиваем таблицу
        table_size = 0; // обнуляем заполненость таблицы

        for (int i = 0; i < old_max_table_size; ++i) { // заполняем таблицу
            if (old_Table[i].status == ONE) {
                add(old_Table[i]);
            }
        }
    }

    void Collisions(Node node, int& index, int& i) { // разрешение коллизий
        int old_index = index;
        while (i < max_table_size) {
            if (table[index].status != ONE || (table[index].fio.fio == node.fio.fio && table[index].app.number_app == node.app.number_app)) {
                table[index] = node;
                table[index].status = ONE;
                table_size++;
                return;
            }
            else {
                index = quadratic_search(index, i++);
            }
        }
        rehash();
        index = old_index;
        i = 0;
        add(node);
    }

public:
    Hash_Table(int initial_max_table_size) : table_size(0), max_table_size(initial_max_table_size) { //Конструктор.  Исходный размер для хэш таблицы. 
        table.assign(max_table_size, Node());
    }

    ~Hash_Table() {} // Деструктор

    void add(Node node) { // вставка узла

        int index = hash_middle_square(node.fio.fio, node.app.number_app); // считаем хэш
        int i = 0; // кол-во попыток разрешить коллизию

        if (table[index].status != ONE) { // первая проверка
            table[index] = node;
            table[index].status = ONE;
            table_size++;
        }
        else {
            Collisions(node, index, i); // разрешение коллизий
        }

        if (table_size >= 0.75 * max_table_size) { // проверка на заполняемость таблицы 75%
            rehash();
        }
    }

    void add_without_read_file(const string& line) { //вставка по строке
        int p1, p2, p3, p4, p5 = 0;
        Node k;
        k.group.l = line[0];
        k.group.num = stoi(line.substr(1, 4));
        p1 = line.find(' ');
        p2 = line.find(' ', p1 + 1);
        p3 = line.find(' ', p2 + 1);
        p4 = line.find(' ', p3 + 1);
        p5 = line.find_last_of(' ');
        k.fio.fio = line.substr(p1 + 1, p4 - p1 - 1);
        k.dir.title = line.substr(p4 + 1, p5 - p4 - 1);
        k.app.number_app = stoi(line.substr(p5 + 1));
        add(k);
    }

    bool search(const string& fio, int num) { // поиск
        int index = hash_middle_square(fio, num);
        int i = 0;

        if (table[index].status == ONE && ((table[index].fio.fio) == fio) && (table[index].app.number_app == num)) {
            return true;
        }
        else {
            while (table[index].status != ZERO && ((table[index].fio.fio) != fio) && (table[index].app.number_app != num)) {
                index = quadratic_search(index, i++);
                if (i > max_table_size) {
                    return false;
                }
                if (table[index].status == ONE && ((table[index].fio.fio) == fio) && (table[index].app.number_app == num)) {
                    return true;
                }

            }
            return false; // Ключ не найден
        }
    }

    bool delete_node(const string& fio, int num) {// Удаление
        int index = hash_middle_square(fio, num);
        int i = 0;
        if (table[index].status == ONE && (table[index].fio.fio == fio) && (table[index].app.number_app == num)) {
            table[index].status = TWO;
            table_size--;
            return true;
        }
        else {
            while (table[index].status != ZERO && ((table[index].fio.fio) != fio) && (table[index].app.number_app != num)) {

                index = quadratic_search(index, i++);
                if (i > max_table_size) {
                    return false;
                }
                if (table[index].status == ONE && (table[index].fio.fio == fio) && (table[index].app.number_app == num)) {
                    table[index].status = TWO;
                    table_size--;
                    return true;
                }
            }
            return false;
        }
    }

    void print() {// печать
        for (int i = 0; i < max_table_size; ++i) {
            cout << "Index " << i << ": ";
            if (table[i].status == ONE) {
                cout << "Key: " << table[i].fio.fio << " " << table[i].app.number_app << "; Line number: " << table[i].line_number << "; All Node: " << table[i].group.l << table[i].group.num << " " << table[i].fio.fio << " " << table[i].dir.title << " " << table[i].app.number_app << "\n";
            }
            else if (table[i].status == TWO) {
                cout << "DELETED" << "\n";
            }
            else {
                cout << "EMPTY" << "\n";
            }
        }
    }

    void read_file(const string& filename) { // чтение файла
        string line;
        int count = 0;
        int p1, p2, p3, p4, p5 = 0;

        ifstream in(filename);
        if (in.is_open()) {

            while ((getline(in, line))) {
                count += 1;
                Node k;
                k.group.l = line[0];
                k.group.num = stoi(line.substr(1, 4));
                p1 = line.find(' ');
                p2 = line.find(' ', p1 + 1);
                p3 = line.find(' ', p2 + 1);
                p4 = line.find(' ', p3 + 1);
                p5 = line.find_last_of(' ');
                k.fio.fio = line.substr(p1 + 1, p4 - p1 - 1);
                k.dir.title = line.substr(p4 + 1, p5 - p4 - 1);
                k.app.number_app = stoi(line.substr(p5 + 1));
                k.line_number = count;
                add(k);
            }
        }
        else {
            cerr << "error";
            return;
        }

        in.close();
    }

    void write_file(const string& filename) { // запись в файл
        ofstream file(filename);

        if (file.is_open()) {
            for (int i = 0; i < max_table_size; ++i) {
                if (table[i].status == ONE) {
                    file << table[i].group.l << table[i].group.num << " " << table[i].fio.fio << " " << table[i].dir.title << " " << table[i].app.number_app << " " << table[i].line_number << " " << "\n";
                }
                else if (table[i].status == TWO) {
                    file << "DELETED" << "\n";
                }
                else {
                    file << "EMPTY" << "\n";
                }
            }
        }
        else {
            cerr << "error";
            return;
        }
        file.close();
    }

};
