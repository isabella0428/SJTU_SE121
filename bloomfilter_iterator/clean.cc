#include <filesystem>
using namespace std;

int main() {
    if (filesystem::exists("./data")) {
        filesystem::remove_all("./data");
    }

    filesystem::create_directories("./data");
}