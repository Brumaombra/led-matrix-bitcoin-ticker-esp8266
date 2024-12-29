import { createRouter, createWebHistory } from 'vue-router';
import LoadingPage from '@/views/LoadingPage.vue';

// Creare il router
const router = createRouter({
    history: createWebHistory('/'),
    routes: [{
        path: '/',
        name: 'loadingPage',
        component: LoadingPage
    }, {
        path: '/app',
        component: () => import('@/views/AppContainer.vue'),

        // Auth children routes
        children: [{
            path: 'wifi',
            name: 'wifi',
            component: () => import('@/views/WiFi.vue')
        }, {
            path: 'apiKey',
            name: 'apiKey',
            component: () => import('@/views/ApiKey.vue')
        }, {
            path: 'settings',
            name: 'settings',
            component: () => import('@/views/Settings.vue')
        }]
    }]
});

export default router;