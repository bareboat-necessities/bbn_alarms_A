#ifndef http_parsing_h
#define http_parsing_h

// Define a structure to hold a key-value pair
struct KeyValuePair {
    String key;
    String value;
};

// Define a maximum number of query arguments
const int MAX_ARGS = 10;

struct HttpRequest {
    String method;
    String path;
    String queryString;
    KeyValuePair queryArgs[MAX_ARGS]; // Array to store key-value pairs
    int queryArgCount = 0; // Counter for the number of query arguments
    String headers;
};

// Function to parse the HTTP GET request
HttpRequest parseHttpRequest(String request) {
    HttpRequest httpRequest;

    // Find the end of the first line (method, path, and query string)
    int firstLineEnd = request.indexOf("\r\n");
    if (firstLineEnd == -1) {
        // Invalid request, return empty struct
        return httpRequest;
    }

    // Extract the first line
    String firstLine = request.substring(0, firstLineEnd);

    // Split the first line into method and path + query string
    int methodEnd = firstLine.indexOf(' ');
    if (methodEnd == -1) {
        // Invalid request, return empty struct
        return httpRequest;
    }

    httpRequest.method = firstLine.substring(0, methodEnd);
    String pathAndQuery = firstLine.substring(methodEnd + 1, firstLine.indexOf(' ', methodEnd + 1));

    // Split path and query string
    int queryStart = pathAndQuery.indexOf('?');
    if (queryStart == -1) {
        // No query string, just the path
        httpRequest.path = pathAndQuery;
        httpRequest.queryString = "";
    } else {
        // Separate path and query string
        httpRequest.path = pathAndQuery.substring(0, queryStart);
        httpRequest.queryString = pathAndQuery.substring(queryStart + 1);
    }

    // Parse query arguments
    if (httpRequest.queryString.length() > 0) {
        int argStart = 0;
        while (argStart < httpRequest.queryString.length() && httpRequest.queryArgCount < MAX_ARGS) {
            int argEnd = httpRequest.queryString.indexOf('&', argStart);
            if (argEnd == -1) {
                argEnd = httpRequest.queryString.length();
            }

            String argPair = httpRequest.queryString.substring(argStart, argEnd);
            int equalSign = argPair.indexOf('=');
            if (equalSign != -1) {
                // Extract key and value
                httpRequest.queryArgs[httpRequest.queryArgCount].key = argPair.substring(0, equalSign);
                httpRequest.queryArgs[httpRequest.queryArgCount].value = argPair.substring(equalSign + 1);
                httpRequest.queryArgCount++; // Increment the argument count
            }

            argStart = argEnd + 1;
        }
    }

    // Extract headers (everything after the first line)
    httpRequest.headers = request.substring(firstLineEnd + 2);

    return httpRequest;
}

#endif
