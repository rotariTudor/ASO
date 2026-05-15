using Microsoft.EntityFrameworkCore;
using TodoApp.Models;

namespace TodoApp.Data;

public class AppDbContext : DbContext
{
    public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

    public DbSet<TodoItem> TodoItems { get; set; }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);

        // Seed cu câteva date inițiale
        modelBuilder.Entity<TodoItem>().HasData(
            new TodoItem
            {
                Id = 1,
                Title = "Configurează proiectul ASP.NET Core",
                Description = "Instalează pachetele NuGet necesare și configurează baza de date.",
                IsCompleted = true,
                Priority = Priority.High,
                CreatedAt = DateTime.Now.AddDays(-3),
                CompletedAt = DateTime.Now.AddDays(-2)
            },
            new TodoItem
            {
                Id = 2,
                Title = "Implementează operațiile CRUD",
                Description = "Creare, citire, actualizare și ștergere pentru task-uri.",
                IsCompleted = false,
                Priority = Priority.High,
                CreatedAt = DateTime.Now.AddDays(-2)
            },
            new TodoItem
            {
                Id = 3,
                Title = "Stilizează interfața cu CSS",
                Description = "Adaugă un design modern și responsiv.",
                IsCompleted = false,
                Priority = Priority.Medium,
                CreatedAt = DateTime.Now.AddDays(-1)
            }
        );
    }
}
