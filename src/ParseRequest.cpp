#include "ParseRequest.h"
#include <sstream>
using namespace std;

HttpRequest parse_request(string request) {
    HttpRequest req;
        istringstream stream(request);
        stream >> req.method >> req.path >> req.version;

        string line;
        while (getline(stream, line)) {
            // Strip trailing carriage return left by getline
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            size_t colon = line.find(":");
            if (colon == string::npos) continue;
            string key = line.substr(0, colon);
            string value = line.substr(colon + 2);
            req.headers[key] = value;
        }
    return req;
}
