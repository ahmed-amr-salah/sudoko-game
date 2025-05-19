import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [vue()],
  server: {
    port: 5173,
    proxy: {
      '/api': {
        target: 'http://localhost:3000',
        changeOrigin: true,
        secure: false,
        ws: true,
      },
      '/sample_sudoku_S.txt': {
        target: 'http://localhost:3000',
        changeOrigin: true,
      },
      '/sample_sudoku_M.txt': {
        target: 'http://localhost:3000',
        changeOrigin: true,
      },
      '/sample_sudoku_H.txt': {
        target: 'http://localhost:3000',
        changeOrigin: true,
      }
    }
  }
})
