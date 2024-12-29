<script setup>
import { ref } from 'vue';

const apiKey = ref('');

// Save the API key
const handleSavePress = () => {
    console.log('API Key saved:', apiKey.value);
};
</script>

<template>
    <div class="flex justify-center">
        <div class="max-w-md w-full bg-white p-8 rounded-lg shadow-lg">
            <!-- Logo and title -->
            <div class="mb-6">
                <div class="flex justify-center mb-6">
                    <img src="/bitcoin.svg" alt="Bitcoin logo" class="h-16 w-16">
                </div>
                <h1 class="text-3xl font-bold text-center text-gray-800">Ticker settings</h1>
            </div>

            <!-- Form -->
            <form @submit.prevent="saveValuesSetting">
                <!-- Visibility -->
                <div>
                    <!-- Title -->
                    <h2 class="text-xl font-bold text-center text-gray-800 mb-5">Visibility</h2>

                    <!-- Current price -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="currentPrice" class="text-sm font-medium text-gray-700">Current price</label>
                        <input v-model="currentPrice" type="checkbox" id="currentPrice" class="toggle-checkbox">
                    </div>

                    <!-- Price change -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="priceChange" class="text-sm font-medium text-gray-700">Price change</label>
                        <input v-model="priceChange" type="checkbox" id="priceChange" class="toggle-checkbox">
                    </div>

                    <!-- Market cap -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="marketCap" class="text-sm font-medium text-gray-700">Market cap</label>
                        <input v-model="marketCap" type="checkbox" id="marketCap" class="toggle-checkbox">
                    </div>

                    <!-- Daily high/low -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="dailyHighLow" class="text-sm font-medium text-gray-700">Daily high/low</label>
                        <input v-model="dailyHighLow" type="checkbox" id="dailyHighLow" class="toggle-checkbox">
                    </div>

                    <!-- Year high/low -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="yearHighLow" class="text-sm font-medium text-gray-700">Year high/low</label>
                        <input v-model="yearHighLow" type="checkbox" id="yearHighLow" class="toggle-checkbox">
                    </div>

                    <!-- Open price -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="openPrice" class="text-sm font-medium text-gray-700">Open price</label>
                        <input v-model="openPrice" type="checkbox" id="openPrice" class="toggle-checkbox">
                    </div>

                    <!-- Volume -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="volume" class="text-sm font-medium text-gray-700">Volume</label>
                        <input v-model="volume" type="checkbox" id="volume" class="toggle-checkbox">
                    </div>
                </div>

                <!-- Divider -->
                <hr class="my-6" />

                <!-- Formatting -->
                <div>
                    <!-- Title -->
                    <h2 class="text-xl font-bold text-center text-gray-800 mb-5">Formatting</h2>
                    
                    <!-- Thousands separator -->
                    <div class="mb-4">
                        <label for="selectFormatType" class="block text-sm font-medium text-gray-700 mb-2">Thousands separator</label>
                        <select id="selectFormatType" class="block w-full px-4 py-3 border border-gray-300 rounded-lg shadow-sm focus:outline-none focus:ring-gray-500 focus:border-gray-500 sm:text-sm">
                            <option value="US">21,000.00</option>
                            <option value="EU">21.000,00</option>
                        </select>
                        <div class="text-sm text-gray-500 mt-2">
                            The formatting will be updated on the next API call
                        </div>
                    </div>
                </div>

                <!-- Save button -->
                <button type="submit" class="w-full py-3 px-4 border border-transparent rounded-lg shadow-sm text-sm font-medium text-white bg-gray-800 hover:bg-gray-600 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-gray-500">
                    Save
                </button>
            </form>
        </div>
    </div>
</template>

<style scoped>
.toggle-checkbox {
    appearance: none;
    width: 2.5rem;
    height: 1.25rem;
    background-color: #d1d5db;
    border-radius: 9999px;
    position: relative;
    cursor: pointer;
    outline: none;
    transition: background-color 0.2s;
}

.toggle-checkbox:checked {
    background-color: rgb(31 41 55 / var(--tw-bg-opacity, 1));
}

.toggle-checkbox:before {
    content: '';
    position: absolute;
    top: 0.125rem;
    left: 0.125rem;
    width: 1rem;
    height: 1rem;
    background-color: white;
    border-radius: 9999px;
    transition: transform 0.2s;
}

.toggle-checkbox:checked:before {
    transform: translateX(1.25rem);
}
</style>