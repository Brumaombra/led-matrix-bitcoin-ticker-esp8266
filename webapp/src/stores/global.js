import { reactive } from 'vue';

export default reactive({
    busy: false, // Global busy state
    networksList: [], // List of networks
    settings: null, // Settings
    messageModal: { // Modal state
        visible: false,
        type: '',
        title: '',
        message: ''
    }
});