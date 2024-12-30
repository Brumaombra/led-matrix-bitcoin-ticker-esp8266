<script setup>
import GlobalStore from '@/stores/global.js';
import { saveSettings, setBusy } from '@/utils/utils.js';
import ToggleSwitch from '@/components/ToggleSwitch.vue';

// Save the API key
const handleSavePress = async () => {
    try {
        setBusy(true); // Busy on
        await saveSettings(GlobalStore.settings);
    } catch (error) {
        console.error(error);
    } finally {
        setBusy(false); // Busy off
    }
};
</script>

<template>
    <div class="flex justify-center">
        <div class="max-w-md w-full bg-white p-8 rounded-lg shadow-lg">
            <!-- Logo and title -->
            <div class="mb-6">
                <div class="flex justify-center mb-6">
                    <img src="/settings.svg" alt="Bitcoin logo" class="h-16 w-16">
                </div>
                <h1 class="text-3xl font-bold text-center text-gray-800">Ticker settings</h1>
            </div>

            <!-- Form -->
            <form @submit.prevent="handleSavePress">
                <!-- Visibility -->
                <div>
                    <!-- Title -->
                    <h2 class="text-xl font-bold text-center text-gray-800 mb-5">Visibility</h2>

                    <!-- Current price -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="currentPrice" class="text-sm font-medium text-gray-700">Current price</label>
                        <ToggleSwitch v-model="GlobalStore.settings.currentPrice" />
                    </div>

                    <!-- Price change -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="priceChange" class="text-sm font-medium text-gray-700">Price change</label>
                        <ToggleSwitch v-model="GlobalStore.settings.priceChange" />
                    </div>

                    <!-- Market cap -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="marketCap" class="text-sm font-medium text-gray-700">Market cap</label>
                        <ToggleSwitch v-model="GlobalStore.settings.marketCap" />
                    </div>

                    <!-- Daily high/low -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="dailyHighLow" class="text-sm font-medium text-gray-700">Daily high/low</label>
                        <ToggleSwitch v-model="GlobalStore.settings.dailyHighLow" />
                    </div>

                    <!-- Year high/low -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="yearHighLow" class="text-sm font-medium text-gray-700">Year high/low</label>
                        <ToggleSwitch v-model="GlobalStore.settings.yearHighLow" />
                    </div>

                    <!-- Open price -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="openPrice" class="text-sm font-medium text-gray-700">Open price</label>
                        <ToggleSwitch v-model="GlobalStore.settings.openPrice" />
                    </div>

                    <!-- Volume -->
                    <div class="mb-4 flex items-center justify-between">
                        <label for="volume" class="text-sm font-medium text-gray-700">Volume</label>
                        <ToggleSwitch v-model="GlobalStore.settings.volume" />
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
                        <select id="selectFormatType" class="block w-full px-4 pr-10 py-3 text-gray-900 bg-white border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-gray-500 focus:border-gray-500 appearance-none bg-[url('data:image/svg+xml;charset=US-ASCII,%3Csvg%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20width%3D%2220%22%20height%3D%2220%22%20viewBox%3D%220%200%2020%2020%22%3E%3Cpath%20fill%3D%22%23666%22%20d%3D%22M5.293%208.293a1%201%200%20011.414%200L10%2011.586l3.293-3.293a1%201%200%20111.414%201.414l-4%204a1%201%200%2001-1.414%200l-4-4a1%201%200%20010-1.414z%22%2F%3E%3C%2Fsvg%3E')] bg-[length:20px] bg-[right_12px_center] bg-no-repeat hover:cursor-pointer" v-model="GlobalStore.settings.formatType">
                            <option value="US">21,000.00</option>
                            <option value="EU">21.000,00</option>
                        </select>
                        <div class="text-sm text-gray-500 mt-2">
                            The formatting will be updated on the next API call
                        </div>
                    </div>
                </div>

                <!-- Save button -->
                <button type="submit" class="w-full py-3 px-4 border border-transparent rounded-lg text-sm font-medium text-white bg-gray-800 hover:bg-gray-600">
                    Save
                </button>
            </form>
        </div>
    </div>
</template>