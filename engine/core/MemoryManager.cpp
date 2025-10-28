//
//  MemoryManager.cpp
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#include "MemoryManager.h"
#include <iostream>
#include <algorithm>

namespace EndKey {
    namespace Core {

        // Static member definitions
        std::unique_ptr<MemoryManager> MemoryManager::instance = nullptr;
        std::mutex MemoryManager::instanceMutex;

        MemoryManager::MemoryManager() {
            // Initialize memory pools
            charPool = std::make_unique<MemoryPool<char, 4096>>();
            intPool = std::make_unique<MemoryPool<int, 1024>>();
            uint32Pool = std::make_unique<MemoryPool<uint32_t, 2048>>();

            // Initialize caches with appropriate sizes
            characterCache = std::make_unique<LRUCache<uint32_t, uint32_t>>(5000);
            macroCache = std::make_unique<LRUCache<std::string, std::string>>(1000);
            vowelCache = std::make_unique<LRUCache<uint16_t, std::vector<uint32_t>>>(500);

            std::cout << "MemoryManager initialized with optimized pools and caches" << std::endl;
        }

        MemoryManager& MemoryManager::getInstance() {
            std::lock_guard<std::mutex> lock(instanceMutex);
            if (!instance) {
                instance = std::unique_ptr<MemoryManager>(new MemoryManager());
            }
            return *instance;
        }

        void MemoryManager::shutdown() {
            std::lock_guard<std::mutex> lock(instanceMutex);
            if (instance) {
                std::cout << "MemoryManager shutting down..." << std::endl;
                std::cout << "Final stats - Allocations: " << instance->totalAllocations.load()
                         << ", Peak memory: " << instance->peakMemoryUsage.load() << " bytes" << std::endl;
                instance.reset();
            }
        }

        // Cache implementations
        void MemoryManager::cacheCharacter(uint32_t key, uint32_t value) {
            if (characterCache) {
                characterCache->put(key, value);
            }
        }

        bool MemoryManager::getCachedCharacter(uint32_t key, uint32_t& value) {
            if (characterCache) {
                return characterCache->get(key, value);
            }
            return false;
        }

        void MemoryManager::cacheMacro(const std::string& key, const std::string& value) {
            if (macroCache) {
                macroCache->put(key, value);
            }
        }

        bool MemoryManager::getCachedMacro(const std::string& key, std::string& value) {
            if (macroCache) {
                return macroCache->get(key, value);
            }
            return false;
        }

        void MemoryManager::cacheVowel(uint16_t key, const std::vector<uint32_t>& value) {
            if (vowelCache) {
                vowelCache->put(key, value);
            }
        }

        bool MemoryManager::getCachedVowel(uint16_t key, std::vector<uint32_t>& value) {
            if (vowelCache) {
                return vowelCache->get(key, value);
            }
            return false;
        }

        // Cache management
        void MemoryManager::clearAllCaches() {
            if (characterCache) characterCache->clear();
            if (macroCache) macroCache->clear();
            if (vowelCache) vowelCache->clear();
        }

        void MemoryManager::clearCharacterCache() {
            if (characterCache) characterCache->clear();
        }

        void MemoryManager::clearMacroCache() {
            if (macroCache) macroCache->clear();
        }

        void MemoryManager::clearVowelCache() {
            if (vowelCache) vowelCache->clear();
        }

        // Memory statistics
        MemoryManager::MemoryStats MemoryManager::getMemoryStats() const {
            MemoryStats stats;
            stats.totalAllocations = totalAllocations.load();
            stats.totalDeallocations = totalDeallocations.load();
            stats.currentAllocations = stats.totalAllocations - stats.totalDeallocations;
            stats.peakMemoryUsage = peakMemoryUsage.load();

            stats.charPoolUsage = charPool ? charPool->getAllocatedCount() : 0;
            stats.intPoolUsage = intPool ? intPool->getAllocatedCount() : 0;
            stats.uint32PoolUsage = uint32Pool ? uint32Pool->getAllocatedCount() : 0;

            stats.characterCacheSize = characterCache ? characterCache->size() : 0;
            stats.macroCacheSize = macroCache ? macroCache->size() : 0;
            stats.vowelCacheSize = vowelCache ? vowelCache->size() : 0;

            return stats;
        }

        void MemoryManager::resetStats() {
            totalAllocations = 0;
            totalDeallocations = 0;
            peakMemoryUsage = 0;
        }

        // Memory optimization
        void MemoryManager::optimizeMemory() {
            // Clear least recently used items from caches if they're getting full
            if (characterCache && characterCache->size() > characterCache->getCapacity() * 0.8) {
                trimCaches();
            }

            // Force garbage collection by creating and destroying temporary objects
            // This helps release memory back to the system
            std::vector<std::unique_ptr<int>> temp;
            for (int i = 0; i < 100; ++i) {
                temp.push_back(allocate<int>());
            }
            temp.clear();
        }

        void MemoryManager::trimCaches() {
            // Reduce cache sizes to 75% of capacity
            if (characterCache) {
                size_t targetSize = characterCache->getCapacity() * 0.75;
                while (characterCache->size() > targetSize) {
                    // LRU cache handles eviction automatically when new items are added
                    characterCache->put(0, 0); // Dummy item to trigger eviction
                }
                characterCache->remove(0); // Remove dummy item
            }

            if (macroCache) {
                size_t targetSize = macroCache->getCapacity() * 0.75;
                while (macroCache->size() > targetSize) {
                    macroCache->put("", ""); // Dummy item
                }
                macroCache->remove(""); // Remove dummy item
            }

            if (vowelCache) {
                size_t targetSize = vowelCache->getCapacity() * 0.75;
                while (vowelCache->size() > targetSize) {
                    vowelCache->put(0, std::vector<uint32_t>{}); // Dummy item
                }
                vowelCache->remove(0); // Remove dummy item
            }
        }

    }
}