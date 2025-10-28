//
//  MemoryManager.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef MemoryManager_h
#define MemoryManager_h

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace EndKey {
    namespace Core {

        /**
         * Memory pool for efficient allocation of frequently used objects
         * Follows RAII pattern for automatic resource management
         */
        template<typename T, size_t PoolSize = 1024>
        class MemoryPool {
        private:
            alignas(T) char pool[PoolSize * sizeof(T)];
            std::vector<bool> allocated;
            size_t nextFree;
            std::mutex mutex;

        public:
            MemoryPool() : allocated(PoolSize, false), nextFree(0) {}

            template<typename... Args>
            std::unique_ptr<T> allocate(Args&&... args) {
                std::lock_guard<std::mutex> lock(mutex);

                for (size_t i = 0; i < PoolSize; ++i) {
                    size_t index = (nextFree + i) % PoolSize;
                    if (!allocated[index]) {
                        allocated[index] = true;
                        nextFree = (index + 1) % PoolSize;

                        T* obj = new(&pool[index * sizeof(T)]) T(std::forward<Args>(args)...);
                        return std::unique_ptr<T>(obj, [this, index](T* ptr) {
                            std::lock_guard<std::mutex> lock(mutex);
                            ptr->~T();
                            allocated[index] = false;
                        });
                    }
                }

                // Fallback to regular allocation if pool is full
                return std::make_unique<T>(std::forward<Args>(args)...);
            }

            size_t getCapacity() const { return PoolSize; }
            size_t getAllocatedCount() const {
                size_t count = 0;
                for (bool a : allocated) if (a) ++count;
                return count;
            }
        };

        /**
         * Cache manager for frequently accessed data
         * Uses LRU eviction policy
         */
        template<typename Key, typename Value>
        class LRUCache {
        private:
            struct Node {
                Key key;
                Value value;
                std::chrono::steady_clock::time_point lastAccessed;
                std::shared_ptr<Node> prev;
                std::shared_ptr<Node> next;
            };

            size_t capacity;
            std::unordered_map<Key, std::shared_ptr<Node>> cache;
            std::shared_ptr<Node> head;
            std::shared_ptr<Node> tail;
            mutable std::mutex mutex;

            void moveToHead(std::shared_ptr<Node> node) {
                if (node == head) return;

                if (node == tail) {
                    tail = node->prev;
                } else if (node->prev) {
                    node->prev->next = node->next;
                }

                if (node->next) {
                    node->next->prev = node->prev;
                }

                node->prev = nullptr;
                node->next = head;

                if (head) {
                    head->prev = node;
                }

                head = node;

                if (!tail) {
                    tail = node;
                }
            }

            void evictLRU() {
                if (tail && cache.size() > capacity) {
                    cache.erase(tail->key);
                    tail = tail->prev;
                    if (tail) {
                        tail->next = nullptr;
                    } else {
                        head = nullptr;
                    }
                }
            }

        public:
            LRUCache(size_t cap = 1000) : capacity(cap) {}

            void put(const Key& key, const Value& value) {
                std::lock_guard<std::mutex> lock(mutex);

                auto it = cache.find(key);
                if (it != cache.end()) {
                    it->second->value = value;
                    it->second->lastAccessed = std::chrono::steady_clock::now();
                    moveToHead(it->second);
                } else {
                    auto node = std::make_shared<Node>();
                    node->key = key;
                    node->value = value;
                    node->lastAccessed = std::chrono::steady_clock::now();

                    cache[key] = node;
                    moveToHead(node);
                    evictLRU();
                }
            }

            bool get(const Key& key, Value& value) {
                std::lock_guard<std::mutex> lock(mutex);

                auto it = cache.find(key);
                if (it != cache.end()) {
                    value = it->second->value;
                    it->second->lastAccessed = std::chrono::steady_clock::now();
                    moveToHead(it->second);
                    return true;
                }
                return false;
            }

            bool remove(const Key& key) {
                std::lock_guard<std::mutex> lock(mutex);

                auto it = cache.find(key);
                if (it != cache.end()) {
                    auto node = it->second;

                    if (node == head) {
                        head = node->next;
                    }
                    if (node == tail) {
                        tail = node->prev;
                    }
                    if (node->prev) {
                        node->prev->next = node->next;
                    }
                    if (node->next) {
                        node->next->prev = node->prev;
                    }

                    cache.erase(it);
                    return true;
                }
                return false;
            }

            void clear() {
                std::lock_guard<std::mutex> lock(mutex);
                cache.clear();
                head = tail = nullptr;
            }

            size_t size() const {
                std::lock_guard<std::mutex> lock(mutex);
                return cache.size();
            }

            size_t getCapacity() const { return capacity; }
        };

        /**
         * Memory Manager for EndKey engine
         * Provides memory pools and caching for optimal performance
         */
        class MemoryManager {
        private:
            static std::unique_ptr<MemoryManager> instance;
            static std::mutex instanceMutex;

            // Memory pools for different object types
            std::unique_ptr<MemoryPool<char, 4096>> charPool;
            std::unique_ptr<MemoryPool<int, 1024>> intPool;
            std::unique_ptr<MemoryPool<uint32_t, 2048>> uint32Pool;

            // Caches for frequently accessed data
            std::unique_ptr<LRUCache<uint32_t, uint32_t>> characterCache;
            std::unique_ptr<LRUCache<std::string, std::string>> macroCache;
            std::unique_ptr<LRUCache<uint16_t, std::vector<uint32_t>>> vowelCache;

            // Memory statistics
            std::atomic<size_t> totalAllocations{0};
            std::atomic<size_t> totalDeallocations{0};
            std::atomic<size_t> peakMemoryUsage{0};

            MemoryManager();

        public:
            ~MemoryManager() = default;

            // Singleton pattern
            static MemoryManager& getInstance();
            static void shutdown();

            // Memory pool access
            template<typename T, typename... Args>
            std::unique_ptr<T> allocate(Args&&... args);

            // Cache access
            void cacheCharacter(uint32_t key, uint32_t value);
            bool getCachedCharacter(uint32_t key, uint32_t& value);

            void cacheMacro(const std::string& key, const std::string& value);
            bool getCachedMacro(const std::string& key, std::string& value);

            void cacheVowel(uint16_t key, const std::vector<uint32_t>& value);
            bool getCachedVowel(uint16_t key, std::vector<uint32_t>& value);

            // Cache management
            void clearAllCaches();
            void clearCharacterCache();
            void clearMacroCache();
            void clearVowelCache();

            // Memory statistics
            struct MemoryStats {
                size_t totalAllocations;
                size_t totalDeallocations;
                size_t currentAllocations;
                size_t peakMemoryUsage;
                size_t charPoolUsage;
                size_t intPoolUsage;
                size_t uint32PoolUsage;
                size_t characterCacheSize;
                size_t macroCacheSize;
                size_t vowelCacheSize;
            };

            MemoryStats getMemoryStats() const;
            void resetStats();

            // Memory optimization
            void optimizeMemory();
            void trimCaches();
        };

    }
}

// Template implementations
namespace EndKey {
    namespace Core {

        template<typename T, typename... Args>
        std::unique_ptr<T> MemoryManager::allocate(Args&&... args) {
            if constexpr (std::is_same_v<T, char>) {
                totalAllocations++;
                auto ptr = charPool->allocate(std::forward<Args>(args)...);
                return std::unique_ptr<T>(static_cast<T*>(ptr.release()));
            } else if constexpr (std::is_same_v<T, int>) {
                totalAllocations++;
                auto ptr = intPool->allocate(std::forward<Args>(args)...);
                return std::unique_ptr<T>(static_cast<T*>(ptr.release()));
            } else if constexpr (std::is_same_v<T, uint32_t>) {
                totalAllocations++;
                auto ptr = uint32Pool->allocate(std::forward<Args>(args)...);
                return std::unique_ptr<T>(static_cast<T*>(ptr.release()));
            } else {
                totalAllocations++;
                return std::make_unique<T>(std::forward<Args>(args)...);
            }
        }

    }
}

#endif /* MemoryManager_h */