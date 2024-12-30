<script setup>
import { onMounted } from 'vue';
import { navTo, getNetworks, getSettings, messageModal } from '@/utils/utils.js';
import GlobalStore from '@/stores/global.js';

// Initial call
const initCall = async () => {
    try {
        GlobalStore.networksList = await getNetworks(); // Get the networks
        GlobalStore.settings = await getSettings(); // Get the settings
    } catch (error) {
        console.error(error); // Log the error
        throw new Error('Failed to load data'); // Throw an error
    }
};

// On component mounted
onMounted(async () => {
    try {
        setTimeout(async () => { // Delay to not overload the ESP
            await initCall(); // Call the initCall function
            navTo('/app/wifi'); // Navigate to the wifi page
        }, 3000);
    } catch (error) {
        console.error(error);
        messageModal('Error', 'Error', 'An error occurred while loading the data');
    }
});
</script>

<template>
    <div class="flex items-center justify-center min-h-screen bg-gray-800">
        <div class="text-center">
            <img src="/bitcoin.svg" alt="Bitcoin logo" class="animate-spin h-24 w-24 mx-auto mb-4">
            <h1 class="text-3xl font-bold text-white">Loading...</h1>
        </div>
    </div>
</template>

<style scoped>
@keyframes spin {
    0% {
        transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
    }
}

.animate-spin {
    animation: spin 1s linear infinite;
}
</style>