using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using TodoApp.Data;
using TodoApp.Models;

namespace TodoApp.Controllers;

public class TodoController : Controller
{
    private readonly AppDbContext _context;

    public TodoController(AppDbContext context)
    {
        _context = context;
    }

    // GET: /Todo
    public async Task<IActionResult> Index(string? filter, string? priority)
    {
        var query = _context.TodoItems.AsQueryable();

        // Filtrare după status
        query = filter switch
        {
            "active"    => query.Where(t => !t.IsCompleted),
            "completed" => query.Where(t => t.IsCompleted),
            _           => query
        };

        // Filtrare după prioritate
        if (Enum.TryParse<Priority>(priority, out var prio))
            query = query.Where(t => t.Priority == prio);

        var items = await query.OrderByDescending(t => t.Priority)
                               .ThenByDescending(t => t.CreatedAt)
                               .ToListAsync();

        ViewBag.Filter   = filter ?? "all";
        ViewBag.Priority = priority ?? "";
        ViewBag.Total     = await _context.TodoItems.CountAsync();
        ViewBag.Active    = await _context.TodoItems.CountAsync(t => !t.IsCompleted);
        ViewBag.Completed = await _context.TodoItems.CountAsync(t => t.IsCompleted);

        return View(items);
    }

    // GET: /Todo/Create
    public IActionResult Create()
    {
        return View(new TodoItem());
    }

    // POST: /Todo/Create
    [HttpPost]
    [ValidateAntiForgeryToken]
    public async Task<IActionResult> Create(TodoItem item)
    {
        if (!ModelState.IsValid)
            return View(item);

        item.CreatedAt = DateTime.Now;
        _context.TodoItems.Add(item);
        await _context.SaveChangesAsync();

        TempData["Success"] = $"Task-ul \"{item.Title}\" a fost adăugat!";
        return RedirectToAction(nameof(Index));
    }

    // GET: /Todo/Edit/5
    public async Task<IActionResult> Edit(int id)
    {
        var item = await _context.TodoItems.FindAsync(id);
        if (item == null) return NotFound();
        return View(item);
    }

    // POST: /Todo/Edit/5
    [HttpPost]
    [ValidateAntiForgeryToken]
    public async Task<IActionResult> Edit(int id, TodoItem item)
    {
        if (id != item.Id) return NotFound();

        if (!ModelState.IsValid)
            return View(item);

        var existing = await _context.TodoItems.FindAsync(id);
        if (existing == null) return NotFound();

        existing.Title       = item.Title;
        existing.Description = item.Description;
        existing.Priority    = item.Priority;

        await _context.SaveChangesAsync();
        TempData["Success"] = "Task-ul a fost actualizat!";
        return RedirectToAction(nameof(Index));
    }

    // POST: /Todo/ToggleComplete/5
    [HttpPost]
    [ValidateAntiForgeryToken]
    public async Task<IActionResult> ToggleComplete(int id)
    {
        var item = await _context.TodoItems.FindAsync(id);
        if (item == null) return NotFound();

        item.IsCompleted  = !item.IsCompleted;
        item.CompletedAt  = item.IsCompleted ? DateTime.Now : null;

        await _context.SaveChangesAsync();
        return RedirectToAction(nameof(Index));
    }

    // POST: /Todo/Delete/5
    [HttpPost]
    [ValidateAntiForgeryToken]
    public async Task<IActionResult> Delete(int id)
    {
        var item = await _context.TodoItems.FindAsync(id);
        if (item == null) return NotFound();

        _context.TodoItems.Remove(item);
        await _context.SaveChangesAsync();

        TempData["Success"] = $"Task-ul \"{item.Title}\" a fost șters.";
        return RedirectToAction(nameof(Index));
    }

    // POST: /Todo/DeleteCompleted
    [HttpPost]
    [ValidateAntiForgeryToken]
    public async Task<IActionResult> DeleteCompleted()
    {
        var completed = _context.TodoItems.Where(t => t.IsCompleted);
        _context.TodoItems.RemoveRange(completed);
        await _context.SaveChangesAsync();

        TempData["Success"] = "Toate task-urile finalizate au fost șterse!";
        return RedirectToAction(nameof(Index));
    }
}
