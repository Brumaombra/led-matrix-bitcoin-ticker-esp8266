<script setup>
import { RouterView } from 'vue-router';
import Navbar from '@/components/Navbar.vue';
</script>

<template>
    <!-- Root container -->
    <div class="flex flex-col h-screen overflow-hidden bg-gray-100">
        <!-- Top Navbar -->
        <Navbar />

        <!-- Scrollable content -->
        <div class="flex-1 overflow-y-auto mt-16 py-5 px-5 md:pt-12 md:px-0">
            <RouterView />
        </div>
    </div>
</template>