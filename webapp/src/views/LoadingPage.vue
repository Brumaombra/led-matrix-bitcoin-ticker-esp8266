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
    <header>
        <img alt="Vue logo" class="logo" src="./assets/bitcoin.svg" width="125" height="125" />
    </header>

    <main>
        <h1>Loading...</h1>
    </main>
</template>