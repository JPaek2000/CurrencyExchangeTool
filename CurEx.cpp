#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp> //for JSON parsing and serialization

using namespace std; 
using json = nlohmann::json;

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

// Function to fetch and parse exchange rates
json fetchExchangeRates(const string& apiKey, const string& baseCurrency) {
    CURL* curl;
    CURLcode res;
    string readBuffer;
    json exchangeRates;

    // Initialize curl sesh
    curl = curl_easy_init();
    if (curl) {
        // Construction of API request URL
        string url = "https://v6.exchangerate-api.com/v6/" + apiKey + "/latest/" + baseCurrency;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); //url fetch
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); //writeback function called for data handling
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer); //readbuffer data destination

        // HTTP request, error catch with Curle_Ok
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "cURL error: " << curl_easy_strerror(res) << endl;
        }

        //Clean the Sesh (release the resources from the curl handle)
        curl_easy_cleanup(curl);

        // Parse JSON data
        try {
            exchangeRates = json::parse(readBuffer);
        } catch (json::parse_error& e) {
            cerr << "JSON parse error: " << e.what() << endl;
        }
    }

    return exchangeRates;
}

// Function to convert an amount using the exchange rate
void convertCurrency(const string& baseCurrency, const string& targetCurrency, double amount) {
    // Fetch exchange rates
    string apiKey = readApiKeyFromFile("api_key.txt");
    if (apiKey.empty()) {
        cerr << "API key status: Invalid" << endl;
        return;
    }

    auto data = fetchExchangeRates(apiKey, baseCurrency);
    if (data.is_null() || !data.contains("conversion_rates")) {
        cerr << "Failed to retrieve or parse exchange rates." << endl;
        return;
    }

    auto rates = data["conversion_rates"];
    if (rates.find(targetCurrency) == rates.end()) {
        cerr << "Invalid target currency." << endl;
        return;
    }

    double rate = rates[targetCurrency].get<double>();
    double convertedAmount = rate * amount;

    cout << amount << " " << baseCurrency << " is equal to " << convertedAmount << " " << targetCurrency << endl;
}

// Main function changed to incorporate simple user input (fixed doesnt just dump all exchange rates anymore)
int main() {
    string baseCurrency;
    string targetCurrency;
    double amount;

    // User input for base currency, target currency, and amount
    cout << "Enter the base currency (e.g., USD): ";
    cin >> baseCurrency;

    cout << "Enter the target currency (e.g., EUR): ";
    cin >> targetCurrency;

    cout << "Enter the amount to convert: ";
    cin >> amount;

    // Convert currency
    convertCurrency(baseCurrency, targetCurrency, amount);

    return 0;
}