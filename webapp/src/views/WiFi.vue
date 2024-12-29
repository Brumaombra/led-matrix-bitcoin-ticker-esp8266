<script setup>
import { ref } from 'vue';

const apiKey = ref('');
const ssid = ref('');
const password = ref('');

// Save the API key
const handleSavePress = () => {
    console.log('API Key saved:', apiKey.value);
};

// Connect to WiFi
const connectToWiFi = () => {
    console.log('Connecting to WiFi with SSID:', ssid.value, 'and Password:', password.value);
};

// Refresh SSID list
const refreshSSIDList = () => {
    console.log('Refreshing SSID list');
    // Add your logic to refresh the SSID list here
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
                <h1 class="text-3xl font-bold text-center text-gray-800">Connect to WiFi</h1>
            </div>

            <!-- Form -->
            <form @submit.prevent="connectToWiFi">
                <!-- SSID -->
                <div class="mb-4">
                    <label for="ssid" class="block text-sm font-medium text-gray-700 mb-2">SSID</label>
                    <div class="flex items-center">
                        <select v-model="ssid" id="ssid" name="ssid" class="block w-full px-4 py-3 border border-gray-300 rounded-lg shadow-sm focus:outline-none focus:ring-gray-500 focus:border-gray-500 sm:text-sm">
                            <option value="" disabled>Select a network</option>
                            <option value="Network1">Network1</option>
                            <option value="Network2">Network2</option>
                        </select>
                        <button type="button" @click="refreshSSIDList" class="ml-2 px-4 py-3 border border-transparent rounded-lg shadow-sm text-sm font-medium text-white bg-gray-800 hover:bg-gray-600 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-gray-500">
                            Refresh
                        </button>
                    </div>
                </div>

                <!-- Password -->
                <div class="mb-4">
                    <label for="password" class="block text-sm font-medium text-gray-700 mb-2">Password</label>
                    <input v-model="password" type="password" id="password" name="password" class="block w-full px-4 py-3 border border-gray-300 rounded-lg shadow-sm focus:outline-none focus:ring-gray-500 focus:border-gray-500 sm:text-sm" placeholder="Enter your WiFi password">
                </div>

                <!-- Connect button -->
                <button type="submit" class="w-full py-3 px-4 border border-transparent rounded-lg shadow-sm text-sm font-medium text-white bg-gray-800 hover:bg-gray-600 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-gray-500">
                    Connect
                </button>
            </form>
        </div>
    </div>
</template>