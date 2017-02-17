#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>
#include <sstream>
#include<string>
#include <iomanip>
#include "../src/data_generator.hpp"
#include "../src/inorder.hpp"
#include "../src/bfs.hpp"
#include "../src/dfs.hpp"
#include "../src/veb.hpp"

using std::cout;
using std::endl;

struct Option{
    std::string name;
    std::string abbreviation;
    std::string description;
    bool is_string;
    bool is_number;
    int int_value;
    long long_value;
    std::string value;
    bool bool_value;
};

std::map<std::string, Option> options;
const std::string verbose = "VERBOSE";
const std::string single = "SINGLE";
const std::string average = "ITERATIONS";
const std::string output = "OUTPUT";
const std::string min_test_size = "MIN_TEST_SIZE";
const std::string max_test_size = "MAX_TEST_SIZE";

struct Algorithm {
    char const *name;
    typedef void (*build_delegate)(int const *inorder, unsigned size, int *&layout, unsigned &layout_size);
    typedef void (*destroy_delegate)(int *layout, unsigned layout_size);
    typedef int const *(*pred_delegate)(int const *layout, unsigned layout_size, int y);
    build_delegate build;
    destroy_delegate destroy;
    pred_delegate pred;
};

std::string str_with_width(std::string str, int width){
    std::string res = str;
    while(res.length() < width){
        res = " " + res;
    }
    return res;
}
std::string str_with_width(long str, int width){
    char buffer[16];
    sprintf(buffer, "%*ld", width, str);
    return buffer;
}
std::string str_with_width(double str, int width)
{
    char buffer[16];
    sprintf(buffer, "%*.2f", width, str);
    return buffer;
}
std::string str_in_center(std::string str, int width){
    std::string res = str;
    while(res.length() < width){
        res = " " + res + " ";
    }
    return res;
}
std::string str_divider(int width){
    std::string res = "";
    while(res.length() < width){
        res += "#";
    }
    return res;
}
void print_header(std::string title, int width){
    std::cout << str_divider(width) << std::endl;
    std::cout << str_in_center(title, width) << std::endl;
    std::cout << str_divider(width) << std::endl;
}

void make_plot_for_results(const int algo_count) {
    std::ofstream plotfile(options[output].value + ".gnuplot");
    plotfile << "set term png\n"
                    "set output '" << options[output].value << ".png'\n"
                    "set ylabel 'time [s]' rotate by 90\n"
                    "set xlabel 'n [# elements]'\n"
                    "set key autotitle columnhead\n"
                    "set title 'Comparing Pred(x) running time'\n"
                    "set key left top\n"
                    "plot for [col=1:" << algo_count << "] '" << options[output].value << ".data' using " <<
                algo_count + 1 << ":col with lines" << endl;
    plotfile.close();
    if(system(("gnuplot " + options[output].value + ".gnuplot").c_str()) != 0){
        cout << "Something went wrong when plotting results" << endl;
    }
    if(system(("rm " + options[output].value + ".gnuplot").c_str()) != 0){
        cout << "Couldn't remove gnuplot file" << endl;
    }
}

void test_running_time() {
    Algorithm algorithms[] =
    {
        { "ino_s", layouts::inorder::build, layouts::inorder::destroy, layouts::inorder::pred_stable },
        { "ino_u", layouts::inorder::build, layouts::inorder::destroy, layouts::inorder::pred_unstable },
        { "bfs_s", layouts::bfs::build, layouts::bfs::destroy, layouts::bfs::pred_stable },
        { "bfs_u", layouts::bfs::build, layouts::bfs::destroy, layouts::bfs::pred_unstable },
        { "dfs_s", layouts::dfs::build, layouts::dfs::destroy, layouts::dfs::pred_stable },
        { "dfs_u", layouts::dfs::build, layouts::dfs::destroy, layouts::dfs::pred_unstable },
        { "veb_s", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred }
    };
    constexpr unsigned algo_size = sizeof(algorithms) / sizeof(algorithms[0]);

    const int col_width = 9;
    const int col_cnt = algo_size + 1;

    print_header("Benchmarking Running Times!", col_width * col_cnt);

    // Open file and print header
    std::ofstream resultFile(options[output].value + ".data");
    for (int i = 0; i < algo_size; ++i) {
        resultFile << algorithms[i].name << ' ';
        std::cout << str_with_width(algorithms[i].name, col_width);
    }
    std::cout << str_with_width("Size", col_width) << std::endl;
    resultFile << "N" << std::endl;

    // start test
    for (long i = options[min_test_size].long_value; i <= options[max_test_size].long_value; i=(int)(i*1.1)) {
        int* numbers = generateArray(i); //Generate data
        int query = generateQuery();
        double times[algo_size] = {0};

        for (unsigned long j = 0; j < algo_size; ++j) {
            int *arr;
            unsigned surrogate;
            algorithms[j].build(numbers, i, arr, surrogate);
            Algorithm::pred_delegate pred = algorithms[j].pred;

            double time = 0;
            std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
            for (long k = 0; k < options[average].int_value; ++k)
                pred(arr, i, generateQuery());
            std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
            times[j] = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() / (double)options[average].int_value;
            algorithms[j].destroy(arr, surrogate);
            resultFile << times[j] << ' ';
            std::cout << str_with_width(times[j], col_width);
        }
        resultFile << i << std::endl;
        std::cout << str_with_width(i, col_width) << std::endl;

    }

    resultFile.close();
    make_plot_for_results((const int) algo_size);
    std::cout << std::endl << "Done testing running times" << std::endl;
}

void print_options(){
    for(auto &o : options){
        Option opt = o.second;
        std::string type = "   ";
        std::string def = "";
        if(opt.is_string){
            type = "string";
            def += opt.value;
        } else if(opt.is_number) {
            type = "int";
            def += str_with_width((long)opt.int_value, 0);
        } else {
            type = "bool";
            def += opt.bool_value ? "yes" : "no";
        }
        std::cout << opt.name << "(" << opt.abbreviation << ")=<" << type << ">\t\t" << opt.description << " => Default: [" << def << "]" << std::endl;
    }
}

void print_help(){
    std::cout << "Benchmark argument takes following arguments:" << std::endl;
    std::cout << "./benchmark input_file <string> [OPTIONALS]" << std::endl;
    std::cout << "Optionals are: " << std::endl;
    print_options();
}

void parse_options(int argc, char* argv[]){
    for(int i = 2; i < argc; i++){
        std::string input = std::string {argv[i]};
        std::size_t found = input.find("=");
        if(found != std::string::npos && found <= input.length()-1){
            //found = sign trying to pass
            std::string key = input.substr(0,found);
            std::string value = input.substr(found + 1, input.length() - found - 1);
            std::cout << "key " << key << " value " << value << std::endl;
            for(auto &o : options){
                Option &opt = o.second;
                if(opt.name.compare(key) == 0 || opt.abbreviation.compare(key) == 0){
                    if(!opt.is_string && !opt.is_number){
                        opt.bool_value = true;
                    } else if(opt.is_string){
                        opt.value = value;
                    } else if(opt.is_number){
                        sscanf(value.c_str(), "%d", &opt.int_value);
                        sscanf(value.c_str(), "%ld", &opt.long_value);
                    }
                }
            }
        } else {
            std::cout << "Skipping " << input << " - couldn't parse it" << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Let's benchmark!" << std::endl;

//            Option: {"long_name", "abbrev", "Desc", is_string, is_int, default_int, default_long, "default", default_bool};
    //optionals
    options[single] = {"single", "s", "Used to specify single algorithm for benchmark", true, false, 0, 0, "", false};
    // default none verbose (Not currently used. Made to test boolean settings)
    options[verbose] = {"verbose", "v", "Sets log level verbose", false, false, 0, 0, ""};
    // default 100 average
    options[average] = {"average", "a", "Set number of iterations to average over", false, true, 100, 0, "", false};
    // default output file: "test_running_time.data"
    options[output] = {"output", "o", "Set the output file name", true, false, 0, 0, "test_running_time", false};
    // default 1000000 as minimum
    options[min_test_size] = {"min_size", "min", "Set minimum build size", false, true, 0, 1000000, "", false};
    // default 5000000 average
    options[max_test_size] = {"max_size", "max", "Set maximum build size", false, true, 0, 5000000, "", false};

    if(argc < 2){
        print_help();
        return 1;
    }

    for(int i = 0; i < argc; i++){
        std::string str = std::string {argv[i]};
        std::size_t found = str.find("help");

        if(found != std::string::npos){
            print_help();
            return 1;
        }
    }

    std::string input_file = std::string {argv[1]};
    parse_options(argc, argv);
    
    test_running_time();

    return 0;
}
