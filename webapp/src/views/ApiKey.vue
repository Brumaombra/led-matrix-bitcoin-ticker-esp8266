<script setup>
import { ref } from 'vue';

const apiKey = ref('');

// Save the API key
const handleSavePress = () => {
    console.log('API Key saved:', apiKey.value);
};
</script>

<template>
    <div class="flex items-center justify-center bg-gray-100">
        <div class="max-w-md w-full bg-white p-8 rounded-lg shadow-lg">
            <!-- Logo and title -->
            <div class="mb-6">
                <div class="flex justify-center mb-6">
                    <img src="/bitcoin.svg" alt="Bitcoin logo" class="h-16 w-16">
                </div>
                <h1 class="text-3xl font-bold text-center text-gray-800">Insert the API Key</h1>
            </div>

            <!-- Form -->
            <form @submit.prevent="handleSavePress">
                <div class="mb-4">
                    <label for="apiKey" class="block text-sm font-medium text-gray-700 mb-2">API Key</label>
                    <input v-model="apiKey" type="text" id="apiKey" name="apiKey" class="block w-full px-4 py-3 border border-gray-300 rounded-lg shadow-sm focus:outline-none focus:ring-gray-500 focus:border-gray-500 sm:text-sm" placeholder="Enter your API key">
                </div>
                <button type="submit" class="w-full py-3 px-4 border border-transparent rounded-lg shadow-sm text-sm font-medium text-white bg-gray-800 hover:bg-gray-600 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:bg-gray-600">
                    Save
                </button>
            </form>
        </div>
    </div>
</template>