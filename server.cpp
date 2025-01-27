#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <unistd.h>
#include <deque>
#include "/home/uwum/gitClones/cpp-httplib/httplib.h"
using namespace std;
using json = nlohmann::json;
string openAiKey;
string openAiRes = "";
string systemPrompt;
string model;
httplib::Client gpt("https://api.openai.com");
string gptPayload;
int contextLevel;



int gptSend() {
  string bearerString = "Bearer " + openAiKey;

  gpt.set_default_headers({
  { "Authorization", bearerString}
  });

  auto res = gpt.Post("/v1/chat/completions", gptPayload, "application/json");
  openAiRes = res->body; 
  return res->status;
}

// Function to generate the JSON payload
json generateChatGPTPayload(const string& previousResponse, const string& userInput) {
// Static deque to maintain conversation history (up to 3 exchanges)
  static deque<pair<string, string>> conversationHistory;

// Add the new exchange to the conversation history
  if (!previousResponse.empty()) {
    conversationHistory.emplace_back(previousResponse, userInput);
  } else {
    conversationHistory.emplace_back("", userInput);
  }

// Limit the history to the last 3 exchanges
  while (conversationHistory.size() > contextLevel) {
    conversationHistory.pop_front();
  }

// Prepare the messages array for the API
  json messages = json::array();

// Add system instructions (optional, can customize)
  messages.push_back({{"role", "system"}, {"content", systemPrompt}});

// Add the conversation history to the JSON payload
  for (const auto& exchange : conversationHistory) {
    if (!exchange.first.empty()) {
      messages.push_back({{"role", "assistant"}, {"content", exchange.first}});
    }
    messages.push_back({{"role", "user"}, {"content", exchange.second}});
  }

// Construct the final JSON payload
  json payload = {
    {"model", model},
    {"messages", messages}
  };

  return payload;
}

void printBanner() {
  cout << " /$$$$$$$                                /$$           /$$  /$$$$$$  /$$                   /$$     \n";
  cout << "| $$__  $$                              |__/          |__/ /$$__  $$| $$                  | $$     \n";
  cout << "| $$  \\ $$  /$$$$$$   /$$$$$$  /$$$$$$$  /$$ /$$$$$$$  /$$| $$  \\__/| $$$$$$$   /$$$$$$  /$$$$$$   \n";
  cout << "| $$$$$$$  /$$__  $$ /$$__  $$| $$__  $$| $$| $$__  $$| $$| $$      | $$__  $$ |____  $$|_  $$_/   \n";
  cout << "| $$__  $$| $$$$$$$$| $$  \\__/| $$  \\ $$| $$| $$  \\ $$| $$| $$      | $$  \\ $$  /$$$$$$$  | $$     \n";
  cout << "| $$  \\ $$| $$_____/| $$      | $$  | $$| $$| $$  | $$| $$| $$    $$| $$  | $$ /$$__  $$  | $$ /$$ \n";
  cout << "| $$$$$$$/|  $$$$$$$| $$      | $$  | $$| $$| $$  | $$| $$|  $$$$$$/| $$  | $$|  $$$$$$$  |  $$$$/ \n";
  cout << "|_______/  \\_______/|__/      |__/  |__/|__/|__/  |__/|__/ \\______/ |__/  |__/ \\_______/   \\___/   \n \n \n \n \n";
}

int main() {
  string previousResponse = "";
  ifstream configFile;
  configFile.open("serverConf.json", ifstream::binary);
  json config = json::parse(configFile);
  printBanner();
  cout << "Hallo! Wilkommen zu meinem Kunstproject BerniniChat. \n \n Sie können nun mit David schreiben!";
  openAiKey = config["openAiKey"];
  systemPrompt = config["systemPrompt"];
  model = config["model"];
  contextLevel = config["contextLevel"];

  while(true) {
    cout << "\nDu: \n";
    string typedIn;
    getline(cin, typedIn);
    gptPayload = generateChatGPTPayload(previousResponse, typedIn).dump(1);
    gptSend();
    json response = json::parse(openAiRes);
    previousResponse = response["choices"][0]["message"]["content"];
    cout << "\nDavid: " << previousResponse << "\n";
  }


}

