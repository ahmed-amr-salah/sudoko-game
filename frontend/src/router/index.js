import { createRouter, createWebHistory } from 'vue-router'
import SudokuBoard from '../components/SudokuBoard.vue'

const routes = [
  {
    path: '/',
    name: 'Home',
    component: SudokuBoard
  }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

export default router 