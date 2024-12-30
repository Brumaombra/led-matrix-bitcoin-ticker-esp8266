<script setup>
import { RouterView } from 'vue-router';
import GlobalStore from '@/stores/global.js';
import Busy from '@/components/Busy.vue';
</script>

<template>
    <div>
		<!-- Main content -->
		<RouterView />

		<!-- Busy indicator -->
		<Busy :show="GlobalStore.busy" />
	</div>
</template>