#include <iostream>
#include <cstdint>
#include <string>

#include "../include/kvstore.h"
#include "../include/test.h"


class CorrectnessTest : public Test
{
private:
	const uint64_t SIMPLE_TEST_MAX = 512;
	const uint64_t LARGE_TEST_MAX = 1024 * 20;

	void regular_test(uint64_t max)
	{
		uint64_t i;

		// Test a single key
		EXPECT(not_found, store.get(1));
		store.put(1, "SE");
		EXPECT("SE", store.get(1));
		EXPECT(true, store.del(1));
		EXPECT(not_found, store.get(1));
		EXPECT(false, store.del(1));

		phase();

		// Test multiple key-value pairs
		for (i = 0; i < max; ++i)
		{
			cout << "put " << i << endl;
			store.put(i, std::string(i + 1, 's'));
			string str1 = store.get(i);
			int length = str1.length();
			int k = str1.size();
			EXPECT(std::string(i + 1, 's'), str1);
			// cout << str1.size() << endl;
		}
		phase();

		// Test after all insertions
		for (i = 0; i < max; ++i)
		{
			cout << "get " << i << endl;
			EXPECT(std::string(i + 1, 's'), store.get(i));
		}
		phase();

		// // Test deletions
		for (i = 0; i < max; i += 2)
		{
			cout << "del1 " << i << endl;
			EXPECT(true, store.del(i));
		}

		for (i = 0; i < max; ++i) {
			cout << "del2 " << i << endl;
			EXPECT((i & 1) ? std::string(i + 1, 's') : not_found,
				   store.get(i));
		}


		for (i = 1; i < max; ++i)
		{
			cout << "del3 " << i << endl;
			EXPECT(i & 1, store.del(i));
		}

		phase();

		report();
	}

public:
	CorrectnessTest(const std::string &dir, bool v = true) : Test(dir, v)
	{
	}

	void start_test(void *args = NULL) override
	{
		std::cout << "KVStore Correctness Test" << std::endl;

		// std::cout << "[Simple Test]" << std::endl;
		// regular_test(SIMPLE_TEST_MAX);

		std::cout << "[Large Test]" << std::endl;
		regular_test(LARGE_TEST_MAX);
	}
};

int main(int argc, char *argv[])
{
	bool verbose = (argc == 2 && std::string(argv[1]) == "-v");

	std::cout << "Usage: " << argv[0] << " [-v]" << std::endl;
	std::cout << "  -v: print extra info for failed tests [currently ";
	std::cout << (verbose ? "ON" : "OFF") << "]" << std::endl;
	std::cout << std::endl;
	std::cout.flush();

	// Here Remind to remake the path
	CorrectnessTest test("./data", verbose);

	test.start_test();

	return 0;
}
