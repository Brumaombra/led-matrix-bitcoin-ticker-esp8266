<script setup>
import { onMounted } from 'vue';
import { navTo } from '@/utils/utils.js';

// On component mounted
onMounted(() => {
    setTimeout(() => {
        navTo('/app');
    }, 2000);
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
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
}
.animate-spin {
    animation: spin 1s linear infinite;
}
</style>