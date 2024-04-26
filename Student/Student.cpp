#include <iostream>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
std::mutex _mutex;
class StudentDB {
public:
    struct Student {
        int id;
        std::string name;
        int age;
    };

    void addStudent(const Student& student) {
        students_.push_back(std::make_shared<Student>(student));
    }

    void delStudent(int id) {
        students_.erase(std::remove_if(students_.begin(), students_.end(),
            [id](const std::shared_ptr<Student>& student) {
                return student->id == id;
            }),
            students_.end());
    }

    std::shared_ptr<Student> getStudent(int id) {
        for (const auto& student : students_) {
            if (student->id == id) {
                return student;
            }
        }
        return nullptr;
    }

private:
    std::vector<std::shared_ptr<Student>> students_;
};

void writeThread(StudentDB& DB, int id, const std::string& name, int age) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::unique_lock<std::mutex> lock(_mutex);
        DB.addStudent({ id, name, age });
        lock.unlock();
        std::cout << "Добавил студента с ID " << id << "\n";
    }
}

void readThread(StudentDB& DB, int id) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::unique_lock<std::mutex> lock(_mutex);
        std::shared_ptr<StudentDB::Student> student = DB.getStudent(id);
        lock.unlock();
        if (student) {
         std::cout << "Информация о студенте с ID " << id << ": " << student->name << ", возраст " << student->age << "\n";
         }
        else {
        std::cout << "Студент с ID " << id << " не найден" << "\n";
         }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    StudentDB DB;

    std::thread writer1(writeThread, std::ref(DB), 1, "Ваня", 20);
    std::thread writer2(writeThread, std::ref(DB), 2, "Маша", 21);
    std::thread reader1(readThread, std::ref(DB), 1);
    std::thread reader2(readThread, std::ref(DB), 2);

    writer1.join();
    writer2.join();
    reader1.join();
    reader2.join();

    return 0;
}