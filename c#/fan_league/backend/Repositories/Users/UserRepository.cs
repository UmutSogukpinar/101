using fan_league.Data;
using fan_league.Entities;
using Microsoft.EntityFrameworkCore;

namespace fan_league.Repositories.Users;

public class UserRepository(AppDbContext dbContext) : IUserRepository
{
    private readonly AppDbContext _dbContext = dbContext;

    public async Task<(IReadOnlyCollection<User> Items, int TotalCount)> GetPagedAsync(
        int pageNumber,
        int pageSize,
        CancellationToken cancellationToken = default)
    {
        IQueryable<User> query = _dbContext.Users.AsNoTracking();
        int totalCount = await query.CountAsync(cancellationToken);
        User[] users = await query
            .OrderBy(user => user.CreatedAt)
            .ThenBy(user => user.Id)
            .Skip((pageNumber - 1) * pageSize)
            .Take(pageSize)
            .ToArrayAsync(cancellationToken);

        return (users, totalCount);
    }

    public Task<User?> GetByIdAsync(
        Guid id,
        CancellationToken cancellationToken = default)
    {
        return _dbContext.Users.FirstOrDefaultAsync(
            user => user.Id == id,
            cancellationToken);
    }

    public async Task AddAsync(
        User user,
        CancellationToken cancellationToken = default)
    {
        await _dbContext.Users.AddAsync(user, cancellationToken);
        await _dbContext.SaveChangesAsync(cancellationToken);
    }

    public async Task UpdateAsync(
        User user,
        CancellationToken cancellationToken = default)
    {
        _dbContext.Users.Update(user);
        await _dbContext.SaveChangesAsync(cancellationToken);
    }

    public async Task<bool> DeleteAsync(
        Guid id,
        CancellationToken cancellationToken = default)
    {
        int deletedCount = await _dbContext.Users
            .Where(user => user.Id == id)
            .ExecuteDeleteAsync(cancellationToken);

        return deletedCount > 0;
    }
}
