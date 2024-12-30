<script setup>
defineProps({
    show: { type: Boolean, default: false }
});
</script>

<template>
    <Transition name="fade">
        <div v-if="show" class="fixed inset-0 z-50 flex items-center justify-center bg-gray-900/50 backdrop-blur-sm">
            <div class="flex flex-col items-center">
                <img src="/bitcoin.svg" alt="Loading" class="h-16 w-16 animate-spin" style="animation-duration: 2s;">
                <span class="mt-4 text-white font-medium">Loading...</span>
            </div>
        </div>
    </Transition>
</template>

<style scoped>
.fade-enter-active,
.fade-leave-active {
    transition: opacity 0.1s ease;
}

.fade-enter-from,
.fade-leave-to {
    opacity: 0;
}
</style>