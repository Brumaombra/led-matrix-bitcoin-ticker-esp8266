<script setup>
import { RouterView } from 'vue-router';
import Navbar from '@/components/Navbar.vue';
</script>

<template>
	<!-- Root container -->
	<div class="root-container">
		<!-- Top Navbar -->
		<Navbar />

		<!-- Scrollable content -->
		<div class="main-content">
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
.main-content {
	flex: 1;
	overflow-y: auto;
	padding: 0;
	margin-top: 60px;
	margin-bottom: 63px;
}
</style>