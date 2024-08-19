#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>

// Callback function handles HTTP request
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to read API key from the api_key.txt file
std::string readApiKeyFromFile(const std::string& fileName) {
    std::ifstream file(fileName);
    std::string apiKey;
    if (file.is_open()) {
        std::getline(file, apiKey);
        file.close();
    } else {
        std::cerr << "Unable to open file " << fileName << std::endl;
    }
    return apiKey;
}

// Main function logic: api key, base currency, target currency, amount to convert. call function, retrieval, calculate, display, and handle errors
int main() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // Read API key from api_key.txt
    std::string apiKey = readApiKeyFromFile("api_key.txt");
    if (apiKey.empty()) {
        std::cerr << "API key status: Invalid" << std::endl;
        return 1;
    }

    curl = curl_easy_init();
    if (curl) {
        std::string baseCurrency = "USD";     // Base currency
        std::string targetCurrency = "EUR";   // Target currency

        std::string url = "https://v6.exchangerate-api.com/v6/" + apiKey + "/latest/" + baseCurrency;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);

        std::cout << "Received data: " << readBuffer << std::endl;
    }

    return 0;
}