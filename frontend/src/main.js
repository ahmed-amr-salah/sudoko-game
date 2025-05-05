import { createApp } from 'vue'
import './style.css'
import App from './App.vue'
import { createVuetify } from 'vuetify'
import 'vuetify/styles'
import '@mdi/font/css/materialdesignicons.css'

const vuetify = createVuetify({
  theme: {
    defaultTheme: 'light',
    themes: {
      light: {
        colors: {
          primary: '#1976D2',
          secondary: '#424242',
          accent: '#FF4081',
          error: '#FF5252',
        },
      },
      dark: {
        colors: {
          primary: '#2196F3',
          secondary: '#757575',
          accent: '#FF4081',
          error: '#FF5252',
        },
      },
    },
  },
})

createApp(App)
  .use(vuetify)
  .mount('#app')
