<script>
import SudokuBoard from './components/SudokuBoard.vue'

export default {
  name: 'App',

  components: {
    SudokuBoard,
  },

  data: () => ({
    isDarkTheme: false,
  }),

  created() {
    // Check user's preferred color scheme
    const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
    this.isDarkTheme = prefersDark;
    this.$vuetify.theme.dark = prefersDark;
  },

  methods: {
    toggleTheme() {
      this.isDarkTheme = !this.isDarkTheme;
      this.$vuetify.theme.dark = this.isDarkTheme;
    }
  },
}
</script>

<template>
  <v-app>
    <v-app-bar app elevation="4" class="app-bar">
      <v-toolbar-title class="app-title">
        <v-icon large class="mr-2">mdi-cube-outline</v-icon>
        Sudoku Solver
      </v-toolbar-title>
      <v-spacer></v-spacer>
      <v-btn icon @click="toggleTheme" class="theme-toggle">
        <v-icon>{{ isDarkTheme ? 'mdi-weather-sunny' : 'mdi-weather-night' }}</v-icon>
      </v-btn>
    </v-app-bar>

    <v-main class="main-content">
      <v-container fluid>
        <SudokuBoard />
      </v-container>
    </v-main>

    <v-footer app padless class="app-footer">
      <v-container>
        <div class="d-flex align-center justify-space-between">
          <span>&copy; {{ new Date().getFullYear() }} - Sudoku Solver</span>
          <div>
            <v-btn icon small href="https://github.com" target="_blank" class="mx-1">
              <v-icon>mdi-github</v-icon>
            </v-btn>
            <v-btn icon small href="https://linkedin.com" target="_blank" class="mx-1">
              <v-icon>mdi-linkedin</v-icon>
            </v-btn>
          </div>
        </div>
      </v-container>
    </v-footer>
  </v-app>
</template>

<style scoped>
.app-bar {
  background: linear-gradient(135deg, #6a11cb 0%, #2575fc 100%) !important;
}

.app-title {
  font-weight: 700;
  letter-spacing: 1px;
}

.theme-toggle {
  transition: transform 0.3s ease;
}

.theme-toggle:hover {
  transform: rotate(30deg);
}

.main-content {
  background-color: var(--v-background-base, #f5f5f5);
  min-height: 100vh;
}

.app-footer {
  background-color: var(--v-surface-base, #ffffff) !important;
  border-top: 1px solid rgba(0, 0, 0, 0.1);
}
</style>
