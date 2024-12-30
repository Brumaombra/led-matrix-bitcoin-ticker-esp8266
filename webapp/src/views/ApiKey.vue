<script setup>
import { ref, computed } from 'vue';
import { saveApiKey, setBusy } from '@/utils/utils.js';

const isFormValid = computed(() => {
    return apiKey.value.trim();
});

const apiKey = ref('');

// Save the API key
const handleSavePress = async () => {
    try {
        setBusy(true); // Busy on
        await saveApiKey(apiKey.value);
    } catch (error) {
        console.error(error);
    } finally {
        setBusy(false); // Busy off
        apiKey.value = ''; // Clear the API key
    }
};
</script>

<template>
    <div class="flex justify-center">
        <div class="max-w-md w-full bg-white p-8 rounded-lg shadow-lg">
            <!-- Logo and title -->
            <div class="mb-6">
                <div class="flex justify-center mb-6">
                    <img src="/key.svg" alt="Bitcoin logo" class="h-16 w-16">
                </div>
                <h1 class="text-3xl font-bold text-center text-gray-800">Insert the API key</h1>
            </div>

            <!-- Form -->
            <form @submit.prevent="handleSavePress">
                <!-- API Key -->
                <div class="mb-4">
                    <label for="apiKey" class="block text-sm font-medium text-gray-700 mb-2">API Key</label>
                    <input v-model="apiKey" type="text" id="apiKey" name="apiKey" class="block w-full px-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-gray-500 focus:border-gray-500" placeholder="Enter your API key">
                </div>

                <!-- Save button -->
                <button type="submit" :disabled="!isFormValid" class="w-full py-3 px-4 border border-transparent rounded-lg text-sm font-medium text-white bg-gray-800 hover:bg-gray-600 disabled:opacity-50 disabled:cursor-not-allowed">
                    Save
                </button>
            </form>
        </div>
    </div>
</template>