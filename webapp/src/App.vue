<script setup>
import { RouterView } from 'vue-router';
import GlobalStore from '@/stores/global.js';
import MessageModal from '@/components/MessageModal.vue';
import Busy from '@/components/Busy.vue';
</script>

<template>
    <div>
		<!-- Main content -->
		<RouterView />

		<!-- Message modal -->
		<MessageModal :show="GlobalStore.messageModal.visible"
			:type="GlobalStore.messageModal.type"
			:title="GlobalStore.messageModal.title"
			:message="GlobalStore.messageModal.message"
			@close="GlobalStore.messageModal.visible = false" />

		<!-- Busy indicator -->
		<Busy :show="GlobalStore.busy" />
	</div>
</template>