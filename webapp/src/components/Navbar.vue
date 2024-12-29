<script setup>
import { navTo } from '@/utils/utils.js';
</script>

<template>
    <nav class="navbar navbar-expand-lg fixed-top bg-body-tertiary">
        <div class="container">
            <a class="navbar-brand">
                <div class="d-flex justify-content-center align-items-center">
                    <img src="/bitcoin.svg" class="navbarLogo me-2" alt="Bitcoin logo" />
                    <p>Bitcoin Ticker</p>
                </div>
            </a>

            <!-- Hamburger for responsive -->
            <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarTicker" aria-controls="navbarTicker" aria-expanded="false" aria-label="Toggle navigation">
                <span class="navbar-toggler-icon"></span>
            </button>

            <!-- Collapse section when mobile -->
            <div id="navbarTicker" class="collapse navbar-collapse">
                <ul class="navbar-nav me-auto mb-2 mb-lg-0">
                    <li class="nav-item">
                        <a id="navLinkApiKey" class="nav-link active" aria-current="page" @click="navTo('/app/apiKey')">API Key</a>
                    </li>
                    <li class="nav-item">
                        <a id="navLinkValues" class="nav-link" @click="navTo('/app/settings')">Settings</a>
                    </li>
                    <li class="nav-item">
                        <a id="navLinkWifi" class="nav-link" @click="navTo('/app/wifi')">WiFi</a>
                    </li>
                </ul>
            </div>
        </div>
    </nav>
</template>