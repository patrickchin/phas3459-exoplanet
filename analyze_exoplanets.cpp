#include <cstring>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <map>

#include <unistd.h>
#include <curl/curl.h>

#include "exoplanet.hpp"

int main(int argc, char *argv[])
{
    clock_t t1 = clock();

    vector<exoplanet> v;
    try {
        string line;
        cin.ignore(numeric_limits<streamsize>::max(),'\n'); // ignore first line
        while (getline(cin, line))
        {
            exoplanet p = exoplanet::from_string(line.cbegin(), line.cend());
            v.push_back(p);
        }
    } catch (exception &e) {
        cerr << " ERROR: " << e.what() << endl;
        return 0;
    }

    clock_t t2 = clock();

    /* Find closest planet */
    {
        exoplanet *closest = nullptr;
        int i = 0, sz = v.size();
        // find first planet with a known distance
        for (; i < sz; ++i)
            if (v[i].distance() >= 0.0)
            {
                closest = &v[i];
                break;
            }
        // compare remaining planets
        for (; i < sz; ++i)
            if (v[i].distance() >= 0.0 && v[i].distance() < closest->distance())
                closest = &v[i];

        if (closest)
            cout << "Closest planet:\n" << *closest << endl;
    }

    clock_t t3 = clock();

    /* Get stats for discovery methods */
    struct discovery_method_stats {
        int count = 0;
        int earliest_year;
        double lightest_mass;
    };

    map<string,discovery_method_stats> count;
    for (auto p : v)
    {
        auto *cur_method = &count[p.method()];
        if (++(*cur_method).count == 1)
        {
            /* new method discovered */
            (*cur_method).earliest_year = p.year();
            (*cur_method).lightest_mass = p.mass();
        }
        else
        {
            if (p.year() < (*cur_method).earliest_year)
                (*cur_method).earliest_year = p.year();
            if (p.mass() < (*cur_method).lightest_mass)
                (*cur_method).lightest_mass = p.mass();
        }
    }

    clock_t t4 = clock();

    for (auto m : count)
    {
        cout << "Method of discovery: " << m.first << endl
            << "    Planets discovered: " << m.second.count << endl
            << "    Earliest discovery: " << m.second.earliest_year << endl
            << "    Lightest planet:    " << m.second.lightest_mass << endl << endl;
    }

    clock_t t5 = clock();

    cout << fixed << setprecision(2)
        << "t1: " << 1000.0 * (t2-t1) / CLOCKS_PER_SEC << " ms\n"
        << "t2: " << 1000.0 * (t3-t2) / CLOCKS_PER_SEC << " ms\n"
        << "t3: " << 1000.0 * (t4-t3) / CLOCKS_PER_SEC << " ms\n"
        << "t4: " << 1000.0 * (t5-t4) / CLOCKS_PER_SEC << " ms\n";

    return 0;
}
