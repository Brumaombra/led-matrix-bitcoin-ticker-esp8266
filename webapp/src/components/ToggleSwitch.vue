<script setup>
// Props
defineProps({
    modelValue: { type: Boolean, required: true }
});

// Emits
defineEmits(['update:modelValue']);
</script>

<template>
    <div class="flex items-center">
        <label class="relative inline-flex items-center cursor-pointer">
            <!-- Input element -->
            <input type="checkbox" class="sr-only peer" :checked="modelValue" @change="$emit('update:modelValue', $event.target.checked)">

            <!-- Background -->
            <div class="w-11 h-6 bg-gray-200 peer-checked:bg-gray-600 rounded-full transition duration-200"></div>

            <!-- Knob -->
            <div class="absolute top-0.5 left-0.5 w-5 h-5 bg-white rounded-full shadow peer-checked:translate-x-5 transition-transform duration-200"></div>
        </label>
    </div>
</template>