import router from '@/router'; // Router object

// Navigate to a path
export const navTo = path => {
    const currentPath = router.currentRoute.value.path; // Get the current path
    if (currentPath === path) return; // Exit if it's the same path
    router.push(path); // Navigate to the path
};