<script setup>
import { RouterView } from 'vue-router';
import Navbar from '@/components/Navbar.vue';
</script>

<template>
	<!-- Root container -->
	<div class="root-container bg-gray-100">
		<!-- Top Navbar -->
		<Navbar />

		<!-- Scrollable content -->
		<div class="pt-0 lg:pt-12">
			<RouterView />
		</div>
	</div>
</template>

<style scoped>
.root-container {
	display: flex;
	flex-direction: column;
	height: 100vh;
	overflow: hidden;
}
</style>