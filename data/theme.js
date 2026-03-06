/**
 * Переключатель темы (как в TKWifiManager): 💻 system → 🌙 dark → ☀️ light
 */
(function () {
  'use strict';
  const KEY = 'tkwm-theme';
  const CYCLE = ['system', 'dark', 'light'];
  const LABELS = { system: '💻', dark: '🌙', light: '☀️' };
  const TITLES = { system: 'Системная тема', dark: 'Тёмная тема', light: 'Светлая тема' };

  function applyTheme(theme) {
    if (theme === 'system') delete document.documentElement.dataset.theme;
    else document.documentElement.dataset.theme = theme;
  }
  function getSaved() {
    try { return localStorage.getItem(KEY) || 'system'; } catch (_) { return 'system'; }
  }
  function setSaved(theme) {
    try { localStorage.setItem(KEY, theme); } catch (_) {}
  }
  applyTheme(getSaved());

  function updateButton(btn, theme) {
    btn.textContent = LABELS[theme];
    btn.setAttribute('title', TITLES[theme]);
  }
  function createButton() {
    var btn = document.createElement('button');
    btn.id = 'tkwm-theme-btn';
    btn.style.cssText = 'position:fixed;bottom:16px;right:16px;z-index:9999;width:40px;height:40px;border-radius:50%;border:1px solid var(--br);background:var(--card);color:var(--ink);font-size:18px;cursor:pointer;display:flex;align-items:center;justify-content:center;padding:0;box-shadow:0 2px 10px rgba(0,0,0,.35);opacity:.7';
    btn.onmouseenter = function () { btn.style.opacity = '1'; };
    btn.onmouseleave = function () { btn.style.opacity = '.7'; };
    btn.onclick = function () {
      var next = CYCLE[(CYCLE.indexOf(getSaved()) + 1) % CYCLE.length];
      setSaved(next);
      applyTheme(next);
      updateButton(btn, next);
    };
    updateButton(btn, getSaved());
    document.body.appendChild(btn);
  }
  if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', createButton);
  else createButton();
})();
