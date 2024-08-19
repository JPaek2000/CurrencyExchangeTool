#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>

using namespace std; 

// Callback function handles HTTP request
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to read API key from the api_key.txt file
string readApiKeyFromFile(const string& fileName) {
    ifstream file(fileName);
    string apiKey;
    if (file.is_open()) {
        getline(file, apiKey);
        file.close();
    } else {
        cerr << "Unable to open file " << fileName << endl;
    }
    return apiKey;
}

// Main function logic: api key, base currency, target currency, amount to convert. call function, retrieval, calculate, display, and handle errors
int main() {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    // Read API key from api_key.txt
    string apiKey = readApiKeyFromFile("api_key.txt");
    if (apiKey.empty()) {
        cerr << "API key status: Invalid" << endl;
        return 1;
    }

    curl = curl_easy_init();
    if (curl) {
        string baseCurrency = "USD";     // Base currency
        string targetCurrency = "EUR";   // Target currency

        string url = "https://v6.exchangerate-api.com/v6/" + apiKey + "/latest/" + baseCurrency;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "cURL error: " << curl_easy_strerror(res) << endl;
        }

        curl_easy_cleanup(curl);

        cout << "Received data: " << readBuffer << endl;
    }

    return 0;
}