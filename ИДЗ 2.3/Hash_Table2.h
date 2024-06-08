#pragma once

#include <iostream> // ����
#include <vector> // ��� �������
#include <fstream> // ��� ������ � ������ �����
#include <string> // ��� ������ �� ��������
using namespace std;

enum Node_status { // ������� �����
    ZERO, ONE, TWO
};

struct Node {
    int line_number; // ��� ������ ������

    Node_status status; // ������ ������

    struct group { // ����� ������
        char l; // ����� ������ ������
        int num; // ����� ������ ������
    }group;

    struct fio { // ��������� ���
        string fio; // ���
    }fio;

    struct Direction { // ��������� �����������
        string title; // �������� �����������
    }dir;

    struct Application { // ��������� ������ ������
        int number_app; // ����� ������
    }app;

    Node() : line_number(0), status(ZERO) {}
};


class Hash_Table {
private:
    const int k1 = 1; // ��������� ��� ������������� ������
    const int k2 = 2; // ��������� ��� ������������� ������
    vector<Node> table; // ���-�������
    int table_size; // ������� ���������� ��������� � �������
    int max_table_size; // ������� �������

    int hash_middle_square(const string fio, int num) { // ��� ������� �������� ��������
        int sum = 0;
        for (char c : fio) { // ��������� ������� ���
            sum += c;
        }
        sum += num; // ���������� ����� ������
        long long square = sum * sum; // �������� � �������
        string square_str = to_string(square); // ��������� � ������
        int squared_len = square_str.length(); // ����� �������� �����
        int key_len = to_string(sum).length();; // ����� �����
        int mid_len = key_len / 2; // ����� ��������
        int start_mid_pos = (squared_len / 2) - mid_len / 2; // ������ ��������
        string mid_str = square_str.substr(start_mid_pos, key_len); // ��������
        int hash = stoi(mid_str); // ����������� � �����           
        return hash % max_table_size;
    }

    int quadratic_search(int index, int i) { // ������������ ����� ����� �������
        return (index + (i * k1) + (k2 * i * i)) % max_table_size;
    }

    void rehash() {
        vector<Node> old_Table = table;
        int old_max_table_size = max_table_size;

        max_table_size = max_table_size * 2; // ����������� ������� � 2 ����
        table.assign(max_table_size, Node()); // ������ � ����������� �������
        table_size = 0; // �������� ������������ �������

        for (int i = 0; i < old_max_table_size; ++i) { // ��������� �������
            if (old_Table[i].status == ONE) {
                add(old_Table[i]);
            }
        }
    }

    void Collisions(Node node, int& index, int& i) { // ���������� ��������
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
    Hash_Table(int initial_max_table_size) : table_size(0), max_table_size(initial_max_table_size) { //�����������.  �������� ������ ��� ��� �������. 
        table.assign(max_table_size, Node());
    }

    ~Hash_Table() {} // ����������

    void add(Node node) { // ������� ����

        int index = hash_middle_square(node.fio.fio, node.app.number_app); // ������� ���
        int i = 0; // ���-�� ������� ��������� ��������

        if (table[index].status != ONE) { // ������ ��������
            table[index] = node;
            table[index].status = ONE;
            table_size++;
        }
        else {
            Collisions(node, index, i); // ���������� ��������
        }

        if (table_size >= 0.75 * max_table_size) { // �������� �� ������������� ������� 75%
            rehash();
        }
    }

    void add_without_read_file(const string& line) { //������� �� ������
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

    bool search(const string& fio, int num) { // �����
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
            return false; // ���� �� ������
        }
    }

    bool delete_node(const string& fio, int num) {// ��������
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

    void print() {// ������
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

    void read_file(const string& filename) { // ������ �����
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

    void write_file(const string& filename) { // ������ � ����
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
