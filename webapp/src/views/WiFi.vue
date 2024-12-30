<script setup>
import { ref } from 'vue';
import { getNetworks, connectToWiFi, setBusy } from '@/utils/utils.js';
import GlobalStore from '@/stores/global.js';

const ssid = ref('');
const password = ref('');
const isLoading = ref(false);

// Connect to WiFi
const handleConnectPress = async () => {
    try {
        setBusy(true); // Busy on
        await connectToWiFi(ssid.value, password.value);
    } catch (error) {
        console.error(error);
    } finally {
        setBusy(false); // Busy off
        password.value = ''; // Clear password
    }
};

// Refresh SSID list
const refreshSSIDList = async () => {
    isLoading.value = true;
    try {
        GlobalStore.networksList = await getNetworks();
    } catch (error) {
        console.error(error);
    } finally {
        isLoading.value = false;
    }
};
</script>

<template>
    <div class="flex justify-center">
        <div class="max-w-md w-full bg-white p-8 rounded-lg shadow-lg">
            <!-- Logo and title -->
            <div class="mb-6">
                <div class="flex justify-center mb-6">
                    <img src="/wifi.svg" alt="Bitcoin logo" class="h-16 w-16">
                </div>
                <h1 class="text-3xl font-bold text-center text-gray-800">Connect to WiFi</h1>
            </div>

            <!-- Form -->
            <form @submit.prevent="handleConnectPress">
                <!-- SSID -->
                <div class="mb-4">
                    <label for="ssid" class="block text-sm font-medium text-gray-700 mb-2">SSID</label>
                    <div class="flex items-center">
                        <!-- Select -->
                        <select v-model="ssid" id="ssid" name="ssid" class="block w-full px-4 pr-10 py-3 text-gray-900 bg-white border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-gray-500 focus:border-gray-500 appearance-none bg-[url('data:image/svg+xml;charset=US-ASCII,%3Csvg%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20width%3D%2220%22%20height%3D%2220%22%20viewBox%3D%220%200%2020%2020%22%3E%3Cpath%20fill%3D%22%23666%22%20d%3D%22M5.293%208.293a1%201%200%20011.414%200L10%2011.586l3.293-3.293a1%201%200%20111.414%201.414l-4%204a1%201%200%2001-1.414%200l-4-4a1%201%200%20010-1.414z%22%2F%3E%3C%2Fsvg%3E')] bg-[length:20px] bg-[right_12px_center] bg-no-repeat hover:cursor-pointer">
                            <option value="">Select a network</option>
                            <option v-for="network in GlobalStore.networksList" :key="network.ssid" :value="network.ssid">
                                {{ network.ssid }} ({{ network.signal }} dBm)
                            </option>
                        </select>

                        <!-- Refresh button -->
                        <button type="button" @click="refreshSSIDList" class="ml-2 px-4 py-3 border border-transparent rounded-lg text-sm font-medium text-white bg-gray-800 hover:bg-gray-600">
                            <div v-if="isLoading">
                                <svg class="animate-spin h-5 w-5 text-white" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
                                    <circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
                                    <path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
                                </svg>
                            </div>
                            <span v-else>Refresh</span>
                        </button>
                    </div>
                </div>

                <!-- Password -->
                <div class="mb-4">
                    <label for="password" class="block text-sm font-medium text-gray-700 mb-2">Password</label>
                    <input v-model="password" type="password" id="password" name="password" class="block w-full px-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-gray-500 focus:border-gray-500" placeholder="Enter your WiFi password">
                </div>

                <!-- Connect button -->
                <button type="submit" class="w-full py-3 px-4 border border-transparent rounded-lg text-sm font-medium text-white bg-gray-800 hover:bg-gray-600">
                    Connect
                </button>
            </form>
        </div>
    </div>
</template>