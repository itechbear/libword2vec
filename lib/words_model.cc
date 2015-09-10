//
// Created by hudong on 9/10/15.
//

#include "words_model.h"

WordsModel::WordsModel()
    : words_(), vectors_() {
}

WordsModel::~WordsModel() {
}

bool WordsModel::ParseFromFile(const std::string &filepath) {
  FILE *vector_file = fopen(filepath.c_str(), "rb");

  assert(nullptr != vector_file);

  uint64_t word_count = 0;
  uint64_t vector_length = 0;

  assert(EOF != fscanf(vector_file, "%lld", &word_count));
  assert(EOF != fscanf(vector_file, "%lld", &vector_length));
  assert(word_count > 0);
  assert(vector_length > 0);

  vectors_.resize(word_count);
  for (auto &item : vectors_) {
    item.resize(vector_length);
  }

  for (int i = 0; i < word_count; i++) {
    char current_char;
    std::string current_token;
    do {
      current_char = static_cast<char>(fgetc(vector_file));
      assert(!feof(vector_file) && !ferror(vector_file));
      if (current_char == '\n') {
        continue;
      }
      if (current_char == ' ') {
        break;
      }
      current_token += current_char;
    } while (true);
    words_.Insert(current_token);

    // Read vectors
    for (int j = 0; j < vector_length; j++) {
      float value = 0.0f;
      fread(&value, sizeof(float), 1, vector_file);
      assert(!feof(vector_file) && !ferror(vector_file));
      vectors_[i][j] = value;
    }

    // Normalize vectors
    float squared_length = 0;
    for (int j = 0; j < vector_length; ++j) {
      squared_length += vectors_[i][j] * vectors_[i][j];
    }
    squared_length = static_cast<float>(sqrt(squared_length));
    for (int j = 0; j < vector_length; ++j) {
      vectors_[i][j] /= squared_length;
    }
  }

  fclose(vector_file);

  // Shrink all vectors to squeeze space cost.
  words_.Shrink();
  for (auto &item : vectors_) {
    item.shrink_to_fit();
  }
  vectors_.shrink_to_fit();

  return true;
}

bool WordsModel::GetNearestWords(const std::string &phrase,
                     const bool concurrent,
                     ResultSet *results) const {
  // Split input phrase
  std::vector<std::string> tokens;
  boost::split(tokens, phrase, boost::is_any_of(" \t"));
  assert(!tokens.empty());
  assert(nullptr != results);

  std::map<std::string, uint64_t> token_positions_;
  const size_t vector_length = static_cast<int>(vectors_[0].size());
  std::vector<float> vector_sum(vector_length, 0);
  for (const auto &token : tokens) {
    if (token_positions_.find(token) != token_positions_.end()) {
      // Duplicated tokens found
      continue;
    }
    const int64_t position = words_.Find(token);
    if (-1 == position) {
      // Erase invalid tokens
      continue;
    }
    token_positions_[token] = static_cast<uint64_t>(position);

    // Add up vectors
    std::transform(vector_sum.begin(), vector_sum.end(),
                   vectors_[position].begin(), vector_sum.begin(),
                   std::plus<float>());

    // Normalize vectors
    float squared_length = 0;
    for (int i = 0; i < vector_length; i++) {
      squared_length += vector_sum[i] * vector_sum[i];
    }
    squared_length = static_cast<float>(sqrt(squared_length));
    for (int i = 0; i < vector_length; i++) {
      vector_sum[i] /= squared_length;
    }
  }
  if (token_positions_.empty()) {
    // No valid tokens available
    return false;
  }

  if (concurrent) {
    ComputeDistancesConcurrently(token_positions_, vector_length, vector_sum, results);
  } else {
    ComputeDistances(token_positions_, vector_length, vector_sum, results);
  }

  return true;
}

void WordsModel::ComputeDistances(const std::map<std::string, uint64_t> &token_positions,
                      const size_t vector_length,
                      const std::vector<float> &vector_sum,
                      ResultSet *results) const {
  // Traverse all words to find nearest words
  const size_t word_count = words_.GetSize();
  for (size_t i = 0; i < word_count; i++) {
    const std::string &word = words_.Get(i);
    // Words in phrase are ignored.
    if (token_positions.find(word) != token_positions.end()) {
      continue;
    }
    float distance = 0;
    for (int j = 0; j < vector_length; j++) {
      distance += vector_sum[j] * vectors_[i][j];
    }
    results->Insert(ResultItem(distance, words_.Get(i)));
  }
}

void WordsModel::ComputeDistancesConcurrently(const std::map<std::string, uint64_t> &token_positions,
                                  const size_t vector_length,
                                  const std::vector<float> &vector_sum,
                                  ResultSet *results) const {
  std::vector<std::thread *> threads;
  const size_t word_count = words_.GetSize();
  const int thread_count = std::thread::hardware_concurrency();
  const int slice_per_thread = ceil(word_count / (double) thread_count);
  std::vector<ResultSet> result_set_vector(thread_count, ResultSet(results->GetCapacity()));
  for (size_t i = 0; i < thread_count; i += slice_per_thread) {
    const int end = std::min<int>(word_count, (i + 1) * slice_per_thread);
    threads.push_back(
        new std::thread(
            std::bind(
                &WordsModel::ComputeDistancesBySlice,
                this,
                token_positions,
                vector_length,
                vector_sum,
                i  * slice_per_thread,
                end,
                &(result_set_vector.at(i))
            )
        )
    );
  }
  for (auto &thread_ptr : threads) {
    thread_ptr->join();
    delete thread_ptr;
  }
  for (auto &result_set : result_set_vector) {
    auto &queue = result_set.GetQueue();
    while (!queue.empty()) {
      results->Insert(queue.top());
      queue.pop();
    }
  }
}

void WordsModel::ComputeDistancesBySlice(const std::map<std::string, uint64_t> &token_positions,
                                         const size_t vector_length,
                                         const std::vector<float> &vector_sum,
                                         const size_t start,
                                         const size_t end,
                                         ResultSet *results) const {
  for (size_t i = start; i < end; i++) {
    const std::string &word = words_.Get(i);
    // Words in phrase are ignored.
    if (token_positions.find(word) != token_positions.end()) {
      continue;
    }
    float distance = 0;
    for (int j = 0; j < vector_length; j++) {
      distance += vector_sum[j] * vectors_[i][j];
    }
    results->Insert(ResultItem(distance, words_.Get(i)));
  }
}
